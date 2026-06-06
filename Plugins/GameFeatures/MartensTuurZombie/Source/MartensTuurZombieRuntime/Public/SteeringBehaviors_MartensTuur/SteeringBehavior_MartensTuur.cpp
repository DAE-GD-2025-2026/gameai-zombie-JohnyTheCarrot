#include "SteeringBehavior_MartensTuur.h"

FVector2D Get2DVec(FVector Vec)
{
	return FVector2D{
		Vec.X,
		Vec.Y
	};
}

FVector Get3DVec(FVector2D Vec)
{
	return FVector{Vec.X, Vec.Y, 0.f};
}

bool USteeringBehavior_Seek_MartensTuur::CheckIfDone(FSteeringOutput_MartensTuur const& Output, float DeltaT,
	FSteeringBehaviorTarget_MartensTuur const& Target, AActor const* Agent) const
{
	return FVector2D::Distance(Get2DVec(Agent->GetActorLocation()), Target.TargetLocation) <= DoneAtDistance;
}

FSteeringOutput_MartensTuur USteeringBehavior_Seek_MartensTuur::CalculateOutput(float DeltaT,
	FSteeringBehaviorTarget_MartensTuur const& Target, ASurvivorPawn const* Agent)
{
	FSteeringOutput_MartensTuur Result;
	
	Result.Direction = 
		Target.TargetLocation - Get2DVec(Agent->GetActorLocation());
	Result.Direction.Normalize();
	Result.FaceDirection = true;
	
	return Result;
}

FSteeringOutput_MartensTuur USteeringBehavior_Arrive_MartensTuur::CalculateOutput(float DeltaT,
	FSteeringBehaviorTarget_MartensTuur const& Target, ASurvivorPawn const* Agent)
{
	auto Output = Super::CalculateOutput(DeltaT, Target, Agent);
	if (auto const Dist = FVector2D::Distance(Get2DVec(Agent->GetActorLocation()), Target.TargetLocation); Dist <= SlowAtDistance)
	{
		Output.SpeedScale = Dist / SlowAtDistance;
	}
	
	return Output;
}

FSteeringOutput_MartensTuur USteeringBehavior_FollowPath_MartensTuur::CalculateOutput(float DeltaT,
	FSteeringBehaviorTarget_MartensTuur const& Target, ASurvivorPawn const* Agent)
{
	if (LastTarget != Target)
	{
		UE_LOG(LogTemp, Warning, TEXT("New target"), Target.TargetLocation.X, Target.TargetLocation.Y);
		LastTarget = Target;
		CurrentPath = Agent->CalculatePath(Get3DVec(Target.TargetLocation));
		for (auto const Vec : CurrentPath)
		{
			UE_LOG(LogTemp, Warning, TEXT("New path vec: %f, %f"), Vec.X, Vec.Y);
		}
		if (CurrentPath.IsEmpty())
			CurrentVec = nullptr;
		else
			CurrentVec = &CurrentPath[0];
	}
	
	if (CurrentVec == nullptr)
	{
		Finish();
		return FSteeringOutput_MartensTuur{};
	}
	
	FSteeringBehaviorTarget_MartensTuur PathTarget{};
	PathTarget.TargetLocation = Get2DVec(*CurrentVec);
	
	UE_LOG(LogTemp, Warning, TEXT("vec: %f, %f"), CurrentVec->X, CurrentVec->Y);
	auto const Output = Super::CalculateOutput(DeltaT, PathTarget, Agent);
	if (Super::CheckIfDone(Output, DeltaT, PathTarget, Agent))
	{
		UE_LOG(LogTemp, Warning, TEXT("done for now!"));
		if (&*CurrentPath.end() == CurrentVec)
		{
			UE_LOG(LogTemp, Warning, TEXT("done"));
			Finish();
			return Output;
		}
		
		UE_LOG(LogTemp, Warning, TEXT("next vec"));
		++CurrentVec;
	}
	return Output;
}

FSteeringOutput_MartensTuur USteeringBehavior_Wander_MartensTuur::CalculateOutput(float DeltaT,
	FSteeringBehaviorTarget_MartensTuur const&, ASurvivorPawn const* Agent)
{
	auto const Angle = FMath::RandRange(MinAngle, MaxAngle);
	FVector2D const AngleVec{FMath::Cos(Angle), FMath::Sin(Angle)};
	
	FVector2D const TargetPos{Get2DVec(Agent->GetActorLocation()) + Get2DVec(Agent->GetActorForwardVector()) * Distance + AngleVec * Radius};
	UE_LOG(LogTemp, Warning, TEXT("Target: %f, %f"), TargetPos.X, TargetPos.Y)
	FSteeringBehaviorTarget_MartensTuur const Target{
		.TargetLocation = TargetPos,
	};
	
	return Super::CalculateOutput(DeltaT, Target, Agent);
}

FSteeringOutput_MartensTuur USteeringBehavior_Flee_MartensTuur::CalculateOutput(float DeltaT,
	FSteeringBehaviorTarget_MartensTuur const& Target, ASurvivorPawn const* Agent)
{
	auto Output = Super::CalculateOutput(DeltaT, Target, Agent);
	
	Output.Direction = -Output.Direction;
	
	return Output;
}

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

FSteeringOutput_MartensTuur USteeringBehavior_Seek_MartensTuur::CalculateOutput(float DeltaT,
                                                                                FSteeringBehaviorTarget_MartensTuur const& Target, AActor const* Agent)
{
	FSteeringOutput_MartensTuur Result;
	
	Result.Direction = 
		Target.TargetLocation - Get2DVec(Agent->GetActorLocation());
	Result.Direction.Normalize();
	Result.FaceDirection = true;
	
	return Result;
}

FSteeringOutput_MartensTuur USteeringBehavior_Wander_MartensTuur::CalculateOutput(float DeltaT,
	FSteeringBehaviorTarget_MartensTuur const&, AActor const* Agent)
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
	FSteeringBehaviorTarget_MartensTuur const& Target, AActor const* Agent)
{
	auto Output = Super::CalculateOutput(DeltaT, Target, Agent);
	
	Output.Direction = -Output.Direction;
	
	return Output;
}

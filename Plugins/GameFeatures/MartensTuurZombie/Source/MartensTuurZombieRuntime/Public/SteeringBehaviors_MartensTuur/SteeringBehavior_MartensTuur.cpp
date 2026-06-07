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
		Output.BlendWeight = Dist / SlowAtDistance;
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
			CurrentVecIdx = NullOpt;
		else
			CurrentVecIdx = 0;
	}
	
	if (!CurrentVecIdx.IsSet())
	{
		Finish();
		return FSteeringOutput_MartensTuur{};
	}
	
	FSteeringBehaviorTarget_MartensTuur PathTarget{};
	auto const CurrentVec = CurrentPath[CurrentVecIdx.GetValue()];
	PathTarget.TargetLocation = Get2DVec(CurrentVec);
	
	UE_LOG(LogTemp, Warning, TEXT("vec: %f, %f"), CurrentVec.X, CurrentVec.Y);
	auto const Output = Super::CalculateOutput(DeltaT, PathTarget, Agent);
	if (Super::CheckIfDone(Output, DeltaT, PathTarget, Agent))
	{
		UE_LOG(LogTemp, Warning, TEXT("done for now!"));
		if (CurrentVecIdx.GetValue() + 1 >= CurrentPath.Num())
		{
			UE_LOG(LogTemp, Warning, TEXT("done"));
			CurrentVecIdx = NullOpt;
			Finish();
			return Output;
		}
		
		UE_LOG(LogTemp, Warning, TEXT("next vec"));
		++CurrentVecIdx.GetValue();
	}
	return Output;
}

FSteeringOutput_MartensTuur USteeringBehavior_Wander_MartensTuur::CalculateOutput(float DeltaT,
	FSteeringBehaviorTarget_MartensTuur const&, ASurvivorPawn const* Agent)
{
	auto const Angle = FMath::RandRange(MinAngle, MaxAngle);
	FVector2D const AngleVec{FMath::Cos(Angle), FMath::Sin(Angle)};
	
	FVector2D const TargetPos{Get2DVec(Agent->GetActorLocation()) + Get2DVec(Agent->GetActorForwardVector()) * Distance + AngleVec * Radius};
	FSteeringBehaviorTarget_MartensTuur const Target{
		.TargetLocation = TargetPos,
	};
	
	return Super::CalculateOutput(DeltaT, Target, Agent);
}

FSteeringOutput_MartensTuur USteeringBehavior_Flee_MartensTuur::CalculateOutput(float DeltaT,
	FSteeringBehaviorTarget_MartensTuur const& Target, ASurvivorPawn const* Agent)
{
	FSteeringOutput_MartensTuur Output{Super::CalculateOutput(DeltaT, Target, Agent)};
	
	Output.Direction = -Output.Direction;
	Output.BlendWeight = 1.f / FVector::Distance(Agent->GetActorLocation(), Get3DVec(Target.TargetLocation));
	
	return Output;
}

void USteeringBehavior_Blended_MartensTuur::OnReset()
{
	for (auto const &Behavior : Behaviors)
	{
		Behavior.Behavior->Reset();
	}
}

TArray<FWeightedBehavior_MartensTuur> const& USteeringBehavior_Blended_MartensTuur::GetBehaviors() const
{
	return Behaviors;
}

FWeightedBehavior_MartensTuur *USteeringBehavior_Blended_MartensTuur::FindBehaviorByKey(FName Key)
{
	return Behaviors.FindByPredicate([&Key] (FWeightedBehavior_MartensTuur const &Behavior)
	{
		return Behavior.Key == Key;
	});
}

bool USteeringBehavior_Blended_MartensTuur::CheckIfDone(FSteeringOutput_MartensTuur const& Output, float DeltaT,
	FSteeringBehaviorTarget_MartensTuur const& Target, AActor const* Agent) const
{
	if (!FinishAuthorityIdx.IsSet()) return false;
	
	return Behaviors[FinishAuthorityIdx.Get(0)].Behavior->IsDone();
}

bool USteeringBehavior_Blended_MartensTuur::AddBehavior(FWeightedBehavior_MartensTuur NewBehavior, bool bIsFinishAuthority)
{
	if (auto *const Existing = FindBehaviorByKey(NewBehavior.Key); Existing != nullptr)
	{
		Existing->bIsEnabled = NewBehavior.bIsEnabled;
		Existing->Weight = NewBehavior.Weight;
		if (bIsFinishAuthority)
			FinishAuthorityIdx = Behaviors.IndexOfByPredicate([&] (FWeightedBehavior_MartensTuur const &Behavior)
			{
				return Behavior.Key == NewBehavior.Key;
			});
		return true;
	}

	auto const Idx = Behaviors.Add(NewBehavior);
	if (bIsFinishAuthority)
		FinishAuthorityIdx = Idx;
	
	return true;
}

void USteeringBehavior_Blended_MartensTuur::SetWeight(FName Key, float NewWeight)
{
	if (auto *const Existing = FindBehaviorByKey(Key); Existing != nullptr)
	{
		Existing->Weight = NewWeight;
	}
}

FSteeringOutput_MartensTuur USteeringBehavior_Blended_MartensTuur::CalculateOutput(float DeltaT,
                                                                                   FSteeringBehaviorTarget_MartensTuur const& Target, ASurvivorPawn const* Agent)
{
	FSteeringOutput_MartensTuur Output;
	
	float TotalWeight = 0.f;
	
	UE_LOG(LogTemp, Warning, TEXT("blended"));
	for (auto const &[_Name, Weight, Behavior, bIsEnabled] : Behaviors)
	{
		if (!Behavior || Weight <= KINDA_SMALL_NUMBER || !bIsEnabled) continue;
		
		FSteeringOutput_MartensTuur const BehaviorOutput = Behavior->CalculateOutput(DeltaT, Target, Agent);
		if (Behavior->CheckIfDone(Output, DeltaT, Target, Agent))
		{
			Behavior->Finish();
			continue;
		} else Behavior->SetNotDone();
	
		Output.Direction += BehaviorOutput.Direction * Weight;
		if (BehaviorOutput.FacingTowards.IsSet())
		{
			Output.FacingTowards
				= Output.FacingTowards.Get(FRotator::ZeroRotator)
				+ BehaviorOutput.FacingTowards.GetValue() * Weight;
		}
		Output.BlendWeight += BehaviorOutput.BlendWeight * Weight;
		TotalWeight += Weight;
		UE_LOG(LogTemp, Warning, TEXT("Behavior: %s, dir {%f, %f} * %f"), *_Name.ToString(), BehaviorOutput.Direction.X, BehaviorOutput.Direction.Y, BehaviorOutput.BlendWeight);
		
		// todo: face direction?
	}
	
	if (TotalWeight > KINDA_SMALL_NUMBER)
	{
		Output.BlendWeight /= TotalWeight;
	}
	Output.Direction = Output.Direction.GetSafeNormal();
	if (Output.FacingTowards.IsSet())
		Output.FacingTowards = Output.FacingTowards.GetValue();
	
	return Output;
}

bool USteeringBehavior_Face_MartensTuur::CheckIfDone(FSteeringOutput_MartensTuur const& Output, float DeltaT,
	FSteeringBehaviorTarget_MartensTuur const& Target, AActor const* Agent) const
{
	auto const CurrentRot = Agent->GetActorRotation();
	
	auto const Angle = FMath::FindDeltaAngleDegrees(
		CurrentRot.Yaw,
		Output.FacingTowards.GetValue().Yaw
	);	
	return Angle <= 5.f;
}

FSteeringOutput_MartensTuur USteeringBehavior_Face_MartensTuur::CalculateOutput(float DeltaT, FSteeringBehaviorTarget_MartensTuur const& Target,
                                                                                ASurvivorPawn const* Agent)
{
	FSteeringOutput_MartensTuur Output;
	auto Dir = Get3DVec(Target.TargetLocation) - Agent->GetActorLocation();
	Dir.Z = 0.f;
	Output.FacingTowards = Dir.Rotation();
	
	return Output;
}

FSteeringOutput_MartensTuur USteeringBehavior_TurnAround_MartensTuur::CalculateOutput(float DeltaT,
	FSteeringBehaviorTarget_MartensTuur const& Target, ASurvivorPawn const* Agent)
{
	if (!InitiallyFacingDirection.IsSet())
	{
		InitiallyFacingDirection = FVector{Agent->GetActorForwardVector().X, Agent->GetActorForwardVector().Y, 0.f};
		TargetRotator = (-InitiallyFacingDirection.GetValue()).Rotation();
	}
	
	FSteeringOutput_MartensTuur Output;
	Output.FacingTowards = TargetRotator;
	return Output;
}

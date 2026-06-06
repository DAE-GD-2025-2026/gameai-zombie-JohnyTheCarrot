#include "SurvivorAgentBehavior_MartensTuur.h"

#include "SteeringBehaviors_MartensTuur/SteeringBehavior_MartensTuur.h"

USurvivorAgentBehavior_MartensTuur::USurvivorAgentBehavior_MartensTuur()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void USurvivorAgentBehavior_MartensTuur::BeginPlay()
{
	Super::BeginPlay();
	FloatingPawnMovement = GetOwner()->GetComponentByClass<UFloatingPawnMovement>();
	check(FloatingPawnMovement);
	UE_LOG(LogTemp, Warning, TEXT("USurvivorAgentBehavior_MartensTuur BeginPlay"));
}

void USurvivorAgentBehavior_MartensTuur::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                                       FActorComponentTickFunction* ThisTickFunction)
{
	if (CurrentSteeringBehavior == nullptr) return;
	
	auto const Output = CurrentSteeringBehavior->CalculateOutput(DeltaTime, SteerTarget, Cast<ASurvivorPawn>(GetOwner()));
	auto const bIsDone = CurrentSteeringBehavior->CheckIfDone(Output, DeltaTime, SteerTarget, GetOwner());
	if (bIsDone) CurrentSteeringBehavior->Finish();
	
	auto Pawn = Cast<APawn>(GetOwner());
	if (Output.Direction.SquaredLength() > 0.f)
	{
		auto const MoveDir = Get3DVec(Output.Direction.GetSafeNormal());
		FVector const Movement{MoveDir * FloatingPawnMovement->GetMaxSpeed()};
		Pawn->AddMovementInput(Movement, Output.SpeedScale);
	
		if (Output.FaceDirection)
		{
			auto const CurrentRot = Pawn->GetActorRotation();
			auto const TargetRot = MoveDir.Rotation();
			
			float const RotationSpeed = 8.f;
			FRotator const NewRot = FMath::RInterpTo(
				CurrentRot,
				TargetRot,
				DeltaTime,
				RotationSpeed
			);
			
			Pawn->SetActorRotation(NewRot);
		}
	}
}

void USurvivorAgentBehavior_MartensTuur::SetCurrentSteeringBehavior(USteeringBehavior_MartensTuur* SteeringBehavior)
{
	check(SteeringBehavior);
	CurrentSteeringBehavior = SteeringBehavior;
	SteeringBehavior->Reset();
}

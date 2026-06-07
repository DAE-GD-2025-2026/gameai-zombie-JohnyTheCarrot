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
	
	auto Pawn = Cast<APawn>(GetOwner());
	if (TargetRotator.IsSet())
	{
		auto const CurrentRot = Pawn->GetActorRotation();
		
		float const RotationSpeed = 8.f;
		FRotator const NewRot = FMath::RInterpTo(
			CurrentRot,
			TargetRotator.GetValue(),
			DeltaTime,
			RotationSpeed
		);
			
		auto const Angle = FMath::FindDeltaAngleDegrees(
			CurrentRot.Yaw,
			TargetRotator.GetValue().Yaw
		);	
		if (Angle <= 2.f)
		{
			TargetRotator.Reset();
		}
		UE_LOG(LogTemp, Warning, TEXT("Angle: %f"), Angle);
		Pawn->SetActorRotation(NewRot);
	}
	
	auto const Output = CurrentSteeringBehavior->CalculateOutput(DeltaTime, SteerTarget, Cast<ASurvivorPawn>(GetOwner()));
	auto const bIsDone = CurrentSteeringBehavior->CheckIfDone(Output, DeltaTime, SteerTarget, GetOwner());
	if (bIsDone)
	{
		CurrentSteeringBehavior->Finish();
		return;
	}
	
	if (Output.Direction.SquaredLength() > KINDA_SMALL_NUMBER)
	{
		auto const MoveDir = Get3DVec(Output.Direction.GetSafeNormal());
		FVector const Movement{MoveDir * FloatingPawnMovement->GetMaxSpeed()};
		Pawn->AddMovementInput(Movement, Output.SpeedScale);
	}
	
	if (Output.FacingTowards.IsSet())
	{
		TargetRotator = Output.FacingTowards.GetValue();
	}
}

USteeringBehavior_MartensTuur* USurvivorAgentBehavior_MartensTuur::GetSteeringBehavior() const
{
	return CurrentSteeringBehavior;
}

void USurvivorAgentBehavior_MartensTuur::SetCurrentSteeringBehavior(USteeringBehavior_MartensTuur* SteeringBehavior)
{
	check(SteeringBehavior);
	CurrentSteeringBehavior = SteeringBehavior;
	CurrentSteeringBehavior->Reset();
}

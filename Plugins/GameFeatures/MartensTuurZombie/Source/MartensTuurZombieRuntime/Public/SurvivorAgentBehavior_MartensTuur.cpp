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
	
	auto const Output = CurrentSteeringBehavior->CalculateOutput(DeltaTime, SteerTarget, GetOwner());
	
	auto Pawn = Cast<APawn>(GetOwner());
	if (Output.Direction.SquaredLength() > 0.f)
	{
		FVector const Movement{Get3DVec(Output.Direction.GetSafeNormal()) * FloatingPawnMovement->GetMaxSpeed()};
		UE_LOG(LogTemp, Warning, TEXT("%f, %f"), Movement.X, Movement.Y);
		Pawn->AddMovementInput(Movement);
	
		if (Output.FaceDirection)
		{
			auto const Rot = Pawn->GetActorRotation() - Get3DVec(Output.Direction).Rotation();
			Pawn->AddControllerYawInput(Rot.Yaw);
		}
	}
}

void USurvivorAgentBehavior_MartensTuur::MoveInDirection(float DeltaTime, FVector2D Direction, float Scale)
{
	FVector const Movement{Get3DVec(Direction) * FloatingPawnMovement->GetMaxSpeed() * Scale * DeltaTime};
	UE_LOG(LogTemp, Warning, TEXT("%f, %f"), Movement.X, Movement.Y);
	Cast<APawn>(GetOwner())->AddMovementInput(Movement);
}

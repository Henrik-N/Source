#include "MyMotionController.h"

#include "MotionControllerComponent.h"

// Sets default values
AMyMotionController::AMyMotionController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MCComponent = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController"));
	SetRootComponent(MCComponent);
	//MCComponent->SetShowDeviceModel(true);
}

void AMyMotionController::SetHand(EControllerHand Hand)
{
	MCComponent->SetTrackingSource(Hand);
	ControllerHand = Hand;
	OnSetHand.Broadcast(Hand);
}

// Called every frame
void AMyMotionController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

#include "MotionControllerPawn.h"

#include "DrawDebugHelpers.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "MotionData.h"
#include "MyMotionController.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetSystemLibrary.h"

#pragma push_macro("PI")

#pragma region Input


// left grab
void AMotionControllerPawn::OnGrabLeftPress()
{
	ControllerMinimalDatas[LEFT_CONTROLLER_INDEX].TriggerKeyEvent(EControllerKeyEvent::GrabPress);
	BroadCastControllerHandPose(LEFT_CONTROLLER_INDEX);
}
void AMotionControllerPawn::OnGrabLeftRelease()
{
	ControllerMinimalDatas[LEFT_CONTROLLER_INDEX].TriggerKeyEvent(EControllerKeyEvent::GrabRelease);
	BroadCastControllerHandPose(LEFT_CONTROLLER_INDEX);
}
// left trigger
void AMotionControllerPawn::OnTriggerLeftPress()
{
	ControllerMinimalDatas[LEFT_CONTROLLER_INDEX].TriggerKeyEvent(EControllerKeyEvent::TriggerPress);
	BroadCastControllerHandPose(LEFT_CONTROLLER_INDEX);
}
void AMotionControllerPawn::OnTriggerLeftRelease()
{
	ControllerMinimalDatas[LEFT_CONTROLLER_INDEX].TriggerKeyEvent(EControllerKeyEvent::TriggerRelease);
	BroadCastControllerHandPose(LEFT_CONTROLLER_INDEX);
}
// right grab
void AMotionControllerPawn::OnGrabRightPress()
{
	ControllerMinimalDatas[RIGHT_CONTROLLER_INDEX].TriggerKeyEvent(EControllerKeyEvent::GrabPress);
	BroadCastControllerHandPose(RIGHT_CONTROLLER_INDEX);
}
void AMotionControllerPawn::OnGrabRightRelease()
{
	ControllerMinimalDatas[RIGHT_CONTROLLER_INDEX].TriggerKeyEvent(EControllerKeyEvent::GrabRelease);
	BroadCastControllerHandPose(RIGHT_CONTROLLER_INDEX);
}
// right trigger
void AMotionControllerPawn::OnTriggerRightPress()
{
	ControllerMinimalDatas[RIGHT_CONTROLLER_INDEX].TriggerKeyEvent(EControllerKeyEvent::TriggerPress);
	BroadCastControllerHandPose(RIGHT_CONTROLLER_INDEX);
}
void AMotionControllerPawn::OnTriggerRightRelease()
{
	ControllerMinimalDatas[RIGHT_CONTROLLER_INDEX].TriggerKeyEvent(EControllerKeyEvent::TriggerRelease);
	BroadCastControllerHandPose(RIGHT_CONTROLLER_INDEX);
}

// * Passing input events through to the Controller state data objects
// *
void AMotionControllerPawn::BroadCastControllerHandPose(uint8 ControllerIndex) const
{
	const auto HandPose = ControllerMinimalDatas[ControllerIndex].HandPose;
	
	if (ControllerIndex == LEFT_CONTROLLER_INDEX)
		LeftController->OnHandPoseUpdate.Broadcast(HandPose);
	else
		RightController->OnHandPoseUpdate.Broadcast(HandPose);
}

void AMotionControllerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// left grab
	PlayerInputComponent->BindAction("GrabLeft", IE_Pressed, this, &AMotionControllerPawn::OnGrabLeftPress);
	PlayerInputComponent->BindAction("GrabLeft", IE_Released, this, &AMotionControllerPawn::OnGrabLeftRelease);
	// left trigger
	PlayerInputComponent->BindAction("TriggerLeft", IE_Pressed, this, &AMotionControllerPawn::OnTriggerLeftPress);
	PlayerInputComponent->BindAction("TriggerLeft", IE_Released, this, &AMotionControllerPawn::OnTriggerLeftRelease);
	// right grab
	PlayerInputComponent->BindAction("GrabRight", IE_Pressed, this, &AMotionControllerPawn::OnGrabRightPress);
	PlayerInputComponent->BindAction("GrabRight", IE_Released, this, &AMotionControllerPawn::OnGrabRightRelease);
	// right trigger
	PlayerInputComponent->BindAction("TriggerRight", IE_Pressed, this, &AMotionControllerPawn::OnTriggerRightPress);
	PlayerInputComponent->BindAction("TriggerRight", IE_Released, this, &AMotionControllerPawn::OnTriggerRightRelease);
}
#pragma endregion // input

void AMotionControllerPawn::GetControllerWorldLocations(FVector(& OutControllerWorldLocations)[CONTROLLER_COUNT]) const
{
	OutControllerWorldLocations[LEFT_CONTROLLER_INDEX] = LeftController->GetActorLocation();
	OutControllerWorldLocations[RIGHT_CONTROLLER_INDEX] = RightController->GetActorLocation();
}

void AMotionControllerPawn::CalculateControllerOffsets(FVector (& ControllerOffsets)[CONTROLLER_COUNT],
                                                  const FVector& HMDLocWorldSpace) const
{
	ControllerOffsets[LEFT_CONTROLLER_INDEX] = HMDLocWorldSpace - LeftController->GetActorLocation();
	ControllerOffsets[RIGHT_CONTROLLER_INDEX] = HMDLocWorldSpace - RightController->GetActorLocation();
}

void AMotionControllerPawn::GetControllerHandPoses(EHandPose (& OutHandPoses)[CONTROLLER_COUNT]) const
{
	for (int i = 0; i < CONTROLLER_COUNT; i++)
		OutHandPoses[i] = ControllerMinimalDatas[i].HandPose;
}

bool AMotionControllerPawn::TryFindMinimalMatch(FMotionStaticSizeData& OutMotion, int32& OutMotionFoundIndex, const TArray<FMotionStaticSizeData>& AvailableMotions, const FControllerMinimalStateHistory& HorizontalHistory, const FControllerMinimalStateHistory& VerticalHistory)
{
	for (int i = 0; i < AvailableMotions.Num(); i++)
	{
		if (AvailableMotions[i].CheckMatch(HorizontalHistory, VerticalHistory))
		{
			OutMotion = AvailableMotions[i];
			OutMotionFoundIndex = i;
			return true;
		}
	}
	return false;
}

void AMotionControllerPawn::TryUpdateControllerDataMinimal(
	bool(& OutUpdated)[CONTROLLER_COUNT],
	const FVector(& ControllerOffsets)[CONTROLLER_COUNT],
	const EHandPose(& HandPoses)[CONTROLLER_COUNT]
	)
{
	const auto& RegionBounds = MotionRegionBounds;
	
	// find the states (regions + HandPoses) the controllers currently are
	//
	uint8 CurrentHorizStates[CONTROLLER_COUNT];
	uint8 CurrentVertStates[CONTROLLER_COUNT];
	for (int i = 0; i < CONTROLLER_COUNT; i++)
	{
		EHorizRegion HorizRegion;
		EVertRegion VertRegion;
		RegionBounds.FindRegions(HorizRegion, VertRegion, ControllerOffsets[i]);
		CurrentHorizStates[i] = MotionState::AsU8(HorizRegion) | MotionState::AsU8(HandPoses[i]);
		CurrentVertStates[i] = MotionState::AsU8(VertRegion) | MotionState::AsU8(HandPoses[i]);
	}
	
	// try to update the history of controller states
	//
	for (int i = 0; i < CONTROLLER_COUNT; i++)
	{
		const bool bHorizHistoryUpdated = ControllerMinimalDatas[i].HorizonalStateHistory.TryUpdateStateHistory(CurrentHorizStates[i]);
		const bool bVertHistoryUpdated = ControllerMinimalDatas[i].VerticalStateHistory.TryUpdateStateHistory(CurrentVertStates[i]);
		OutUpdated[i] = bHorizHistoryUpdated | bVertHistoryUpdated;
	}
}

AMotionControllerPawn::AMotionControllerPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	VROrigin = CreateDefaultSubobject<USceneComponent>(TEXT("VR Origin"));
	VROrigin->SetMobility(EComponentMobility::Movable);
	SetRootComponent(VROrigin);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(VROrigin);
}

void AMotionControllerPawn::BeginPlay()
{
	Super::BeginPlay();

	UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor); // for Oculus

	const FAttachmentTransformRules AttachmentTransformRules
	(
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::KeepWorld,
		false
	);
	const FTransform SpawnTransform = FTransform(FRotator::ZeroRotator,FVector::ZeroVector,FVector::OneVector);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (!MotionControllerBPClass)
		UE_LOG(LogTemp, Error, TEXT("MotionControllerPawn.cpp: No BP Class set for the controllers."))

	/// Spawn right hand
	RightController = GetWorld()->SpawnActor<AMyMotionController>(MotionControllerBPClass, SpawnTransform, SpawnParams);
	// Attach controllers root to the same root as the camera, so both the locations will be relative to the VROrigin
	RightController->AttachToComponent(VROrigin, AttachmentTransformRules);
	RightController->SetHand(EControllerHand::Right);
	RightController->SetOwner(this);

	/// Spawn left hand
	LeftController = GetWorld()->SpawnActor<AMyMotionController>(MotionControllerBPClass, SpawnTransform, SpawnParams);
	LeftController->AttachToComponent(VROrigin, AttachmentTransformRules);
	LeftController->SetHand(EControllerHand::Left);
	LeftController->SetOwner(this);


	if (AvailableMotionsContainer)
		AvailableMotionsContainer->Init();
	else
		UE_LOG(LogTemp, Error, TEXT("No MotionsContainer set for player"));
}

void AMotionControllerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// properties on the pawn
	// function start
	const FVector HMDLocWorldSpace = Camera->GetComponentLocation();
	
	FVector ControllerWorldLocations[CONTROLLER_COUNT];
	GetControllerWorldLocations(ControllerWorldLocations);
	
	FVector ControllerOffsets[CONTROLLER_COUNT];
	CalculateControllerOffsets(ControllerOffsets, HMDLocWorldSpace);
	
	EHandPose HandPoses[CONTROLLER_COUNT];
	GetControllerHandPoses(HandPoses);
	
	bool bUpdated[CONTROLLER_COUNT];
	TryUpdateControllerDataMinimal(bUpdated, ControllerOffsets, HandPoses);

	
	for (int ControllerIndex = 0; ControllerIndex < CONTROLLER_COUNT; ControllerIndex++)
	{
		if (!bUpdated[ControllerIndex]) continue;

		// for controllers with an updated minimal history, update the fat history as well
		//
		auto& FatData = ControllerFatDatas[ControllerIndex];

		//// calculate radians turned
		////
		//// ignore Z / height
		const FVector2D ControllerOffsetXY = static_cast<FVector2D>(ControllerOffsets[ControllerIndex]);
		const FVector2D ControllerDirXY = ControllerOffsetXY.GetSafeNormal();

		//// Calculate how many radians the controller has rotated around the HMD since the last horizontal update
		////
		const float CosTheta =
			FVector2D::DotProduct(ControllerDirXY, FatData.DirectionToControllerXY) /
				(ControllerDirXY.Size() * FatData.DirectionToControllerXY.Size()); // todo size squared

		const float RadiansTurned = FMath::Acos(CosTheta);

		UE_LOG(LogTemp, Warning, TEXT("Radians turned: %f"), RadiansTurned);

		
		//// update the saved controller direction to the current one
		FatData.DirectionToControllerXY = ControllerDirXY;

		//
		FatData.Update(ControllerWorldLocations[ControllerIndex], RadiansTurned, EControllerTilt::Ignore); // todo Controller tilt
		
		// todo calculate hand tilt
		//
		
		// for controllers with an updated history, check if the history matches any defined motion
		//
		const TArray<FMotionStaticSizeData>& AvailableMotionsStaticSizeData = AvailableMotionsContainer->MotionsStaticSizeData;

		using MotionIndex = int32;
		TArray<TTuple<EControllerHand, MotionIndex>> FoundMotion;
	
		// for each update motion triggered, broadcast an event that triggers the motions ability
		FMotionStaticSizeData MotionFound;
		int32 MotionMatchFoundIndex;
		if (AMotionControllerPawn::TryFindMinimalMatch(
			MotionFound,
			MotionMatchFoundIndex,
			AvailableMotionsStaticSizeData,
			ControllerMinimalDatas[ControllerIndex].HorizonalStateHistory,
			ControllerMinimalDatas[ControllerIndex].VerticalStateHistory))
		{
			const FMotionDynamicSizeData& MinimalMatchedMotionDynamicData = AvailableMotionsContainer->MotionsDynamicSizeData[MotionMatchFoundIndex];
			
			UE_LOG(LogTemp, Warning, TEXT("MINIMAL MATCH! (regions + handpose matches)"));
			
			float RadiansTurnedHistory[8];
			FVector ControllerLocationHistory[8];
			FatData.GetControllerHistoryDataInOrder(RadiansTurnedHistory, ControllerLocationHistory);

			if (MinimalMatchedMotionDynamicData.CheckMatch(RadiansTurnedHistory))
			{
				UE_LOG(LogTemp, Warning, TEXT("COMPLETE MOTION MATCH! (with turn radians as well!)"));
				// Broadcast event with parameters motion index and ControllerLocationHistory,
				// 	triggering some action from the motion.
			}
		}
	}
}

AMyMotionController* AMotionControllerPawn::GetRightMotionController()
{
	return RightController;
}

AMyMotionController* AMotionControllerPawn::GetLeftMotionController()
{
	return LeftController;
}

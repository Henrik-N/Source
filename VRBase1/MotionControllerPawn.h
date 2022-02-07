#pragma once

#include "CoreMinimal.h"

#include "DA_Motions.h"
#include "MotionData.h"

#include "MotionState.h"

#include "GameFramework/Pawn.h"
#include "MotionControllerPawn.generated.h"

constexpr uint8 LEFT_CONTROLLER_INDEX = 0;
constexpr uint8 RIGHT_CONTROLLER_INDEX = 1;
constexpr uint8 CONTROLLER_COUNT = 2;

constexpr EControllerHand AsControllerHandEnum(uint8 ControllerIndex)
{
	if (ControllerIndex == LEFT_CONTROLLER_INDEX)
		return EControllerHand::Left;
	return EControllerHand::Right;
}

class AMyMotionController;
class UCameraComponent;

/// Event dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnFireAction, int, ActionIndex, AMyMotionController*, MotionController,
                                               FTransform, SpawnTransform);

UCLASS()
class VRBASE1_API AMotionControllerPawn : public APawn
{
	GENERATED_BODY()

private:
	void GetControllerWorldLocations(FVector (& OutControllerWorldLocations)[CONTROLLER_COUNT]) const;
	void CalculateControllerOffsets(FVector (& ControllerOffsets)[CONTROLLER_COUNT], const FVector& HMDLocWorldSpace) const;
	void GetControllerHandPoses(EHandPose (& OutHandPoses)[CONTROLLER_COUNT]) const;
	
	void TryUpdateControllerDataMinimal(
		bool (& OutUpdated)[CONTROLLER_COUNT],
		const FVector (& ControllerOffsets)[CONTROLLER_COUNT],
		const EHandPose (& HandPoses)[CONTROLLER_COUNT]
		);
	
	
	static bool TryFindMinimalMatch(
		FMotionStaticSizeData& OutMotion,
		int32& OutMotionFoundIndex,
		const TArray<FMotionStaticSizeData>& AvailableMotions, 
		const FControllerMinimalStateHistory& HorizontalHistory,
		const FControllerMinimalStateHistory& VerticalHistory
		);
	
	
public:
	AMotionControllerPawn();

	UPROPERTY(EditAnywhere)
	UDA_Motions* AvailableMotionsContainer; 

	
	UPROPERTY(EditAnywhere)
	FMotionRegionBounds MotionRegionBounds{};
	
	// Checked and maybe updated every frame
	UPROPERTY()
	FControllerMinimalData ControllerMinimalDatas[CONTROLLER_COUNT] {FControllerMinimalData{}, FControllerMinimalData{}};
	
	// Updated only the frames FControllerMinimalData gets updated
	UPROPERTY()
	FControllerFatData ControllerFatDatas[CONTROLLER_COUNT] {FControllerFatData{}, FControllerFatData{}};
	
protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/// Event dispatchers
	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
	FOnFireAction OnFireAction;

	/// Config
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AMyMotionController> MotionControllerBPClass;

	/// Components
	UPROPERTY(BlueprintReadWrite)
	USceneComponent* RootComp;

	UPROPERTY(BlueprintReadWrite)
	USceneComponent* VROrigin;

	UPROPERTY(BlueprintReadWrite)
	UCameraComponent* Camera;

	/// References
	UPROPERTY(BlueprintReadOnly)
	AMyMotionController* LeftController;

	UPROPERTY(BlueprintReadOnly)
	AMyMotionController* RightController;

	UFUNCTION(BlueprintCallable)
	AMyMotionController* GetRightMotionController();

	UFUNCTION(BlueprintCallable)
	AMyMotionController* GetLeftMotionController();

private:
	void BroadCastControllerHandPose(uint8 ControllerIndex) const;
	// *** CONTROLLER INPUT EVENTS ***
	// these are just pass-through functions because you cant use UObject method delegates 
	void OnGrabLeftPress();
	void OnGrabRightPress();
	
	void OnGrabLeftRelease();
	void OnGrabRightRelease();

	void OnTriggerLeftPress();
	void OnTriggerRightPress();

	void OnTriggerLeftRelease();
	void OnTriggerRightRelease();
};

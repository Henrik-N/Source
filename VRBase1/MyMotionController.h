#pragma once

#include "CoreMinimal.h"

#include "MotionData.h"
#include "MotionState.h"
#include "GameFramework/Actor.h"
#include "MyMotionController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSetHand, EControllerHand, Hand);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHandPoseUpdate, EHandPose, HandPose);


class UMotionControllerComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class VRBASE1_API AMyMotionController : public AActor
{
	GENERATED_BODY()

public:
	AMyMotionController();

	UFUNCTION()
	void SetHand(EControllerHand Hand);

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
    FOnHandPoseUpdate OnHandPoseUpdate;
	
	// For inverting Z on the mesh for the left hand
	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
	FOnSetHand OnSetHand;

	UPROPERTY(BlueprintReadWrite)
	TEnumAsByte<EHandPoses> CurrentHandPose = EHandPoses::OpenHand;

	UPROPERTY(BlueprintReadWrite)
	EHandPose HandPose = EHandPose::OpenHand;
	
	/// ** COMPONENTS ***
	UPROPERTY(VisibleAnywhere)
	UMotionControllerComponent* MCComponent;

private:
	UPROPERTY()
	EControllerHand ControllerHand;
public:
	/// *** PUBLIC FUNCTIONS ***
	virtual void Tick(float DeltaTime) override;
};

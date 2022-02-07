#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum EHandPoses
{
	Ignore UMETA(DisplayName, "Ignore"),
	OpenHand UMETA(DisplayName, "Open Hand"),
	DoublePointing UMETA(DiplayName, "Double Pointing"),
	ClosedHand UMETA(DisplayName, "Closed Hand"),
};

UENUM()
enum ERegionType
{
	Horizontal UMETA(DisplayName, "Horizontal"),
	Vertical UMETA(DisplayName, "Vertical"),
	Either UMETA(DisplayName, "Either")
};

#include "MotionData.generated.h"

USTRUCT()
struct FHorizData
{
	GENERATED_USTRUCT_BODY()

	void Init(int32 InRegion, float InThetaAroundAngle, float InThetaOffsetAllowance, EHandPoses InHandPose);
	bool Compare(const FHorizData& MotionToTriggerAbility) const;

	UPROPERTY()
	int32 Region = 0;

	UPROPERTY()
	TEnumAsByte<EHandPoses> HandPose = EHandPoses::OpenHand;
	
	UPROPERTY()
	float TurnRadians = 0.f;
	
	/// How many radians away from this motions ThetaAround angle is tolerated in a Compare-call 
        /// for it to be considered "equal" another motion?
	UPROPERTY()
	float TurnRadiansOffsetAllowance = 0.f;

	UPROPERTY()
	FVector Location = FVector::ZeroVector; // World space. Updates when entering AND when leaving an area

	friend bool operator==(const FHorizData& Left, const FHorizData& Right);
};

USTRUCT()
struct FVertiData
{
	GENERATED_USTRUCT_BODY()

	void Init(int32 InRegion, EHandPoses InHandPose);
	bool Compare(const FVertiData& MotionToTriggerAbility) const;

	UPROPERTY()
	int32 Region = 0;
	
	UPROPERTY()
	TEnumAsByte<EHandPoses> HandPose = EHandPoses::OpenHand;

	friend bool operator==(const FVertiData& Left, const FVertiData& Right);
};


USTRUCT()
struct FHandMotion
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TArray<FHorizData> HorizontalData;

	UPROPERTY()
	TArray<FVertiData> VerticalData;

	friend bool operator==(const FHandMotion& Left, const FHandMotion& Right);
};

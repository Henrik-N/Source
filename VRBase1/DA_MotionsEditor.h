#pragma once
#include "MotionState.h"

#include "DA_MotionsEditor.generated.h"

enum class EHandPose : uint8;
enum class EHorizRegion : uint8;
USTRUCT()
struct FHorizRegionListing
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	EHorizRegion HorizontalRegion{};

	UPROPERTY(EditDefaultsOnly)
	EHandPose HandPose{};

	UPROPERTY(EditDefaultsOnly, Category = "Test", meta = (ClampMin = 0, ClampMax = 359, UIMin = 0, UIMax = 359))
	float TurnAtLeastDegrees = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Test", meta = (ClampMin = 0, ClampMax = 359, UIMin = 0, UIMax = 359))
	float TurnAtMostDegrees = 359;
	
	uint8 RegionHandPoseAsU8() const { return MotionState::AsU8(HorizontalRegion) | MotionState::AsU8(HandPose); }
	void GetTurnDataInRadians(float& TurnRadians, float& TurnRadiansOffsetAllowance) const;
};

USTRUCT()
struct FVertRegionListing
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	EVertRegion VerticalRegion{};

	UPROPERTY(EditDefaultsOnly)
	EHandPose HandPose{};

	uint8 AsU8() const { return MotionState::AsU8(VerticalRegion) | MotionState::AsU8(HandPose); }
};

UCLASS()
class UDA_Motion : public UDataAsset
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Regions")
	TArray<FHorizRegionListing> HorizontalRegions{};
	
	UPROPERTY(EditDefaultsOnly, Category = "Regions")
	TArray<FVertRegionListing> VerticalRegions{};
public:
	void GetStaticSizeDataPacked(uint64& HorizBits, uint64& VertBits, uint8& HorizBitCount, uint8& VertBitCount) const;

	void GetDynamicSizeDataPacked(
			TArray<float>& TurnRadiansEachHorizRegion,
			TArray<float>& TurnRadiansOffsetAllowanceEachHorizRegion
		) const;
};

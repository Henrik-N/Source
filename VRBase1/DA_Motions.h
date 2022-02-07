#pragma once


#include "DA_MotionsEditor.h"

#include "DA_Motions.generated.h"


USTRUCT()
struct FMotionStaticSizeData // 18 bytes
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere)
	uint64 HorizBits;

	UPROPERTY(VisibleAnywhere)
	uint64 VertBits;

	UPROPERTY(VisibleAnywhere)
	uint8 HorizBitsCount; // number of bits that matter in HorizBits

	UPROPERTY(VisibleAnywhere)
	uint8 VertBitsCount; // number of bits that matter in VertBits
	
	explicit FMotionStaticSizeData() = default;
	explicit FMotionStaticSizeData(const UDA_Motion* MotionListing);
	
	FMotionStaticSizeData(const FMotionStaticSizeData& ToCopy);
	bool CheckMatch(const FControllerMinimalStateHistory& HorizontalHistory,
	                const FControllerMinimalStateHistory& VerticalHistory) const;

	bool operator==(const FMotionStaticSizeData& Other) const;
};

USTRUCT()
struct FMotionDynamicSizeData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	TArray<float> TurnRadiansEachHorizRegion;
	
	UPROPERTY(VisibleAnywhere)
	TArray<float> TurnRadiansOffsetAllowanceEachHorizRegion;
	
	explicit FMotionDynamicSizeData() = default;
	explicit FMotionDynamicSizeData(const UDA_Motion* MotionListing);

	bool CheckMatch(const float (& TurnRadiansHistory)[8]) const;
};


UCLASS()
class UDA_Motions : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY()
	TArray<FMotionStaticSizeData> MotionsStaticSizeData{};

	UPROPERTY()
	TArray<FMotionDynamicSizeData> MotionsDynamicSizeData{};

	void Init();

protected:
	UPROPERTY(EditDefaultsOnly)
	TArray<UDA_Motion*> MotionDataAssets{};
	
private:
	// Creates the packed arrays of data based on the contents of the data assets the MotionDataAssets array points to
	void UpdateMotionArrays();
};

#include "DA_MotionsEditor.h"

void FHorizRegionListing::GetTurnDataInRadians(float& TurnRadians, float& TurnRadiansOffsetAllowance) const
{
	const float TurnAtLeastRad = FMath::DegreesToRadians(TurnAtLeastDegrees);
	const float TurnAtMostRad = FMath::DegreesToRadians(TurnAtMostDegrees);
		
	TurnRadians = TurnAtLeastRad;
	TurnRadiansOffsetAllowance = TurnAtMostRad - TurnAtLeastRad;
}

void UDA_Motion::GetStaticSizeDataPacked(uint64& HorizBits, uint64& VertBits, uint8& HorizBitCount, uint8& VertBitCount) const
{
	checkf(HorizontalRegions.Num() <= 8, TEXT("Regions count must be <= 8"));
	checkf(VerticalRegions.Num() <= 8, TEXT("Regions count must be <= 8"));

	HorizBits = 0;
	HorizBitCount = 0;
	for (const FHorizRegionListing& HorizRegion : HorizontalRegions)
	{
		HorizBits <<= 8;
		HorizBits |= HorizRegion.RegionHandPoseAsU8();
		HorizBitCount++;
	}

	VertBits = 0;
	VertBitCount = 0;
	for (const FVertRegionListing& VertRegion : VerticalRegions)
	{
		VertBits <<= 8;
		VertBits |= VertRegion.AsU8();
		VertBitCount++;
	}	
}

void UDA_Motion::GetDynamicSizeDataPacked(
		TArray<float>& TurnRadiansEachHorizRegion,
		TArray<float>& TurnRadiansOffsetAllowanceEachHorizRegion
	) const
{
	const int32 HorizRegionCount = HorizontalRegions.Num();

	TurnRadiansEachHorizRegion.Init(0, HorizRegionCount);
	TurnRadiansOffsetAllowanceEachHorizRegion.Init(0, HorizRegionCount);
	
	for (int i = 0; i < HorizRegionCount; i++)
	{
		HorizontalRegions[i].GetTurnDataInRadians(
			TurnRadiansEachHorizRegion[i],
			TurnRadiansOffsetAllowanceEachHorizRegion[i]
			);
	}
}

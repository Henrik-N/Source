#include "DA_Motions.h"

#include "DA_MotionsEditor.h"
#include "Kismet/KismetSystemLibrary.h"

FMotionStaticSizeData::FMotionStaticSizeData(const UDA_Motion* MotionListing)
{
	HorizBits = 0;
	VertBits = 0;
	HorizBitsCount = 0;
	VertBitsCount = 0;
	
	MotionListing->GetStaticSizeDataPacked(HorizBits, VertBits, HorizBitsCount, VertBitsCount);
}

FMotionStaticSizeData::FMotionStaticSizeData(const FMotionStaticSizeData& ToCopy)
{
	HorizBits = ToCopy.HorizBits;
	VertBits = ToCopy.VertBits;
	HorizBitsCount = ToCopy.HorizBitsCount;
	VertBitsCount = ToCopy.VertBitsCount;
}

bool FMotionStaticSizeData::CheckMatch(
	const FControllerMinimalStateHistory& HorizontalHistory,
	const FControllerMinimalStateHistory& VerticalHistory) const
{
	if (HorizBitsCount > 0)
	{
		const bool bHorizMatch = MotionState::FilterLastStates(HorizontalHistory.History, HorizBitsCount) == HorizBits;

		UE_LOG(LogTemp, Warning, TEXT("Horiz History: %llu"), HorizontalHistory.History);
		UE_LOG(LogTemp, Warning, TEXT("Horiz This: %llu"), HorizBits);
		
		if (!bHorizMatch) return false;
	}
	
	if (VertBitsCount > 0)
	{
		const bool bVertMatch = MotionState::FilterLastStates(VerticalHistory.History, VertBitsCount) == VertBits;
		UE_LOG(LogTemp, Warning, TEXT("Vert History: %llu"), VerticalHistory.History);
		UE_LOG(LogTemp, Warning, TEXT("Vert This: %llu"), VertBits);
		if (!bVertMatch) return false;
	}
	
	return true;
}

bool FMotionStaticSizeData::operator==(const FMotionStaticSizeData& Other) const
{
	return HorizBits == Other.HorizBits && VertBits == Other.VertBits
	&& HorizBitsCount == Other.HorizBitsCount && VertBitsCount && Other.VertBitsCount;
}

FMotionDynamicSizeData::FMotionDynamicSizeData(const UDA_Motion* MotionListing)
{
	TurnRadiansEachHorizRegion = TArray<float>();
	TurnRadiansOffsetAllowanceEachHorizRegion = TArray<float>();
	MotionListing->GetDynamicSizeDataPacked(TurnRadiansEachHorizRegion, TurnRadiansOffsetAllowanceEachHorizRegion);
}


bool FMotionDynamicSizeData::CheckMatch(const float(& TurnRadiansHistory)[8]) const
{
	for (int i = 0; i < TurnRadiansEachHorizRegion.Num(); i++)
	{
		const bool bTurnWithInBoundsOfData = FMath::Abs(TurnRadiansHistory[i] - TurnRadiansEachHorizRegion[i]) < TurnRadiansOffsetAllowanceEachHorizRegion[i];
		UE_LOG(LogTemp, Warning, TEXT("Turnradians history: %f :: TurnRadians for the motion: %f :: TurnRadiansOffSetAllowance for the motion: %f"),
			TurnRadiansHistory[i],
			TurnRadiansEachHorizRegion[i],
			TurnRadiansOffsetAllowanceEachHorizRegion[i]);

		if (!bTurnWithInBoundsOfData) return false;
	}
	return true;
}

void UDA_Motions::Init()
{
	UpdateMotionArrays();
}

void UDA_Motions::UpdateMotionArrays()
{
	const auto ListingCount = MotionDataAssets.Num();
	MotionsStaticSizeData.Init(FMotionStaticSizeData{}, ListingCount);
	MotionsDynamicSizeData.Init(FMotionDynamicSizeData{}, ListingCount);
	
	for (int i = 0; i < ListingCount; i++)
	{
		MotionsStaticSizeData[i] = FMotionStaticSizeData(MotionDataAssets[i]);
		
		MotionsDynamicSizeData[i] = FMotionDynamicSizeData(MotionDataAssets[i]);

		UE_LOG(LogTemp, Warning, TEXT("Dynamic data: "));
		
		for (int k = 0; k < MotionsDynamicSizeData[i].TurnRadiansEachHorizRegion.Num(); k++)
		{
			UE_LOG(LogTemp, Warning, TEXT("Dynamic data: TurnRadians: %f :: OffsetAllowance: %f"),
				MotionsDynamicSizeData[i].TurnRadiansEachHorizRegion[k],
				MotionsDynamicSizeData[i].TurnRadiansOffsetAllowanceEachHorizRegion[k]
				)
		}
	}
}






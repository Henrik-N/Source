#include "MotionState.h"

bool FControllerMinimalStateHistory::TryUpdateStateHistory(uint8 NewState)
{
	// If the new region is the same as the last one in the history, we won't update
	//
	const uint8 LastRegion = History & MotionState::RegionMask;
	const uint8 NewRegion = NewState & MotionState::RegionMask;

	if (LastRegion == NewRegion)
		return false;

	// For the previous state, keep the region, but update the HandPose to the current one,
	//	as the registered HandPose already set will be the one we had when we entered the region.
	//	Now we want to set it to the one we had when we left the region.
	//
	History &= ~(History & MotionState::HandPoseMask); // clear registered HandPose
	History |= NewState & MotionState::HandPoseMask; // register current HandPose

	// Shift the last state to the left and put the new state in it's place
	//
	History <<= 8;
	History |= NewState;

	return true;
}


EHandPose FControllerKeysState::EvaluateHandPose() const
{
	if (bHoldingTrigger)
		return EHandPose::ClosedHand;
	if (bHoldingGrab)
		return EHandPose::PointingHand;
	return EHandPose::OpenHand;
}


void FControllerMinimalData::TriggerKeyEvent(EControllerKeyEvent KeyEvent)
{
	switch (KeyEvent)
	{
		case EControllerKeyEvent::GrabPress: KeysState.bHoldingGrab = true; break;
		case EControllerKeyEvent::GrabRelease: KeysState.bHoldingGrab = false; break;
		case EControllerKeyEvent::TriggerPress: KeysState.bHoldingTrigger = true; break;
		case EControllerKeyEvent::TriggerRelease: KeysState.bHoldingTrigger = false; break;
		default: ;
	}
	HandPose = KeysState.EvaluateHandPose();
}


void FControllerFatData::UpdateCurrentIndex()
{
	CurrentIndex = (CurrentIndex + 1) % 8;
}

void FControllerFatData::Update(const FVector& ControllerLocation, float RadiansTurned,
	EControllerTilt ControllerTilt)
{
	UpdateCurrentIndex();

	ControllerWorldLocationHistory[CurrentIndex] = ControllerLocation;
	RadiansTurnedAroundPlayerHistory[CurrentIndex] = RadiansTurned;

	ControllerTiltHistory <<= 8;
	ControllerTiltHistory |= static_cast<uint8>(ControllerTilt);
}

void FControllerFatData::GetControllerHistoryDataInOrder(float(& OutRadiansTurnedAroundPlayerHistory)[8],
	FVector(& OutControllerWorldLocationHistory)[8]) const
{
	auto StartIndex = static_cast<int8>(CurrentIndex);

	for (int i = 0; i < 8; i++)
	{
		OutRadiansTurnedAroundPlayerHistory[i] = RadiansTurnedAroundPlayerHistory[StartIndex];
		OutControllerWorldLocationHistory[i] = ControllerWorldLocationHistory[StartIndex];
		
		StartIndex--;
		if (StartIndex < 0)
			StartIndex = 7;
	}
}


void FMotionRegionBounds::FindRegions(EHorizRegion& HorizontalRegion, EVertRegion& VerticalRegion,
                                      const FVector ControllerOffset) const
{
	HorizontalRegion = FindHorizontalRegion(ControllerOffset);
	VerticalRegion = FindVerticalRegion(ControllerOffset);
}

EHorizRegion FMotionRegionBounds::FindHorizontalRegion(const FVector& ControllerOffset) const
{
	const FVector ControllerOffsetXY = FVector(ControllerOffset.X, ControllerOffset.Y, 0.f);
	const float DistanceToController = FMath::Abs(ControllerOffsetXY.Size()); // TODO size squared
	
	// If the distance to the controller with ignored Z is lower than InnerBounds, set region to InnerRegion.
	//	And so on.
	//
	if (DistanceToController < InnerBound)
		return EHorizRegion::InnerRegion;
	
	if (DistanceToController > OuterBound)
		return EHorizRegion::OuterRegion;

	return EHorizRegion::MiddleRegion;
}

EVertRegion FMotionRegionBounds::FindVerticalRegion(const FVector& ControllerOffset) const
{
	const float ControllerHeightOffset = ControllerOffset.Z;

	if (ControllerHeightOffset > UpperBound)
		return EVertRegion::HighRegion;
	
	if (ControllerHeightOffset > LowerBound)
		return EVertRegion::MiddleRegion;
	
	return EVertRegion::LowRegion;
}


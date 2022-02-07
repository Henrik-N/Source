// Fill out your copyright notice in the Description page of Project Settings.


#include "MotionData.h"


bool FHorizData::Compare(const FHorizData& MotionToTriggerAbility) const
{
	/// Check if hand poses match
	if (HandPose != MotionToTriggerAbility.HandPose)
	{
		return false;
	}

	/// Check if horizontal regions match
	const bool bRegionsMatch = Region == MotionToTriggerAbility.Region;
	/// Check if turns match
	bool bTurnsMatch = false;
	const float OtherTurnRadians = MotionToTriggerAbility.TurnRadians;

	if (FMath::IsNearlyEqual(OtherTurnRadians, 0.f))
		// todo: Turn check should always be disabled if TurnRadians is set to 0. 
	{
		bTurnsMatch = true;
	}

	else if (FMath::Abs(TurnRadians - OtherTurnRadians) < MotionToTriggerAbility.TurnRadiansOffsetAllowance ||
		FMath::Abs(TurnRadians - OtherTurnRadians) < TurnRadiansOffsetAllowance)
	{
		bTurnsMatch = true;
	}

	return bRegionsMatch && bTurnsMatch;
}


void FHorizData::Init(int32 InRegion, float InThetaAroundAngle, float InThetaOffsetAllowance, EHandPoses InHandPose)
{
	Region = InRegion;
	TurnRadians = InThetaAroundAngle;
	TurnRadiansOffsetAllowance = InThetaOffsetAllowance;
	HandPose = InHandPose;
}

void FVertiData::Init(int32 InRegion, EHandPoses InHandPose)
{
	Region = InRegion;
	HandPose = InHandPose;
}

bool FVertiData::Compare(const FVertiData& MotionToTriggerAbility) const
{
	/// Check if hand poses match

	if (HandPose != MotionToTriggerAbility.HandPose)
	{
		return false;
	}

	if (Region != MotionToTriggerAbility.Region)
	{
		return false;
	}

	return true;
}

bool operator==(const FVertiData& Left, const FVertiData& Right)
{
	const bool bRegion = Left.Region == Right.Region;
	const bool bHandPose = Left.HandPose == Right.HandPose;
	return bRegion && bHandPose;
}

bool operator==(const FHorizData& Left, const FHorizData& Right)
{
	const bool bRegions = Left.Region == Right.Region;
	const bool bHandPoses = Left.HandPose == Right.HandPose;
	const bool bTurnRadians = Left.TurnRadians == Right.TurnRadians;
	const bool bTurnRadiansOffsetAllowance = Left.TurnRadiansOffsetAllowance == Right.TurnRadiansOffsetAllowance;
	return bRegions && bHandPoses && bTurnRadians && bTurnRadiansOffsetAllowance;
}

bool operator==(const FHandMotion& Left, const FHandMotion& Right)
{
	const bool bHorizDataEqual = Left.HorizontalData == Right.HorizontalData;
	const bool bVertiDataEqual = Left.VerticalData == Right.VerticalData;
	return bHorizDataEqual && bVertiDataEqual;
}

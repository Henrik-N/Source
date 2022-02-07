#pragma once

#include "BitMacro.h"

#include "MotionState.generated.h"


UENUM(meta = (BitFlags))
enum class EHorizRegion : uint8
{
	None = 0 UMETA(Hidden),
	InnerRegion = BIT(0) UMETA(DisplayName, "Inner region"),
	MiddleRegion = BIT(1) UMETA(DisplayName, "Middle horizontal region"),
	OuterRegion = BIT(2) UMETA(DisplayName "Outer region"),
};
ENUM_CLASS_FLAGS(EHorizRegion);

UENUM(meta = (BitFlags))
enum class EVertRegion : uint8
{
	None = 0 UMETA(Hidden),
	LowRegion = BIT(0) UMETA(DisplayName, "Low region"),
	MiddleRegion = BIT(1) UMETA(DisplayName, "Middle vertical region"),
	HighRegion = BIT(2) UMETA(DisplayName, "High region"),
};
ENUM_CLASS_FLAGS(EVertRegion);

UENUM(BlueprintType, meta = (BitFlags))
enum class EHandPose : uint8
{
	None = 0 UMETA(Hidden),
	OpenHand = BIT(3),
	PointingHand = BIT(4),
	ClosedHand = BIT(5), 
};
ENUM_CLASS_FLAGS(EHandPose);

/// A chain of uint8s packed as a uint64
USTRUCT()
struct FControllerMinimalStateHistory
{
	GENERATED_BODY()

	UPROPERTY()
	uint64 History = 0;

	// Shifts PreviousRegions 8 bits to the left and puts NewRegion in the place that opens up.
	//	Only updates if NewRegion is different from the last bits, returns true if it updated anything.
	bool TryUpdateStateHistory(uint8 NewState);
};

enum class EControllerKeyEvent : uint8
{
	Null = 0,
	GrabPress,
	GrabRelease,
	TriggerPress,
	TriggerRelease,
};

USTRUCT()
struct FControllerKeysState
{
	GENERATED_BODY()
	UPROPERTY()
	bool bHoldingGrab = false;

	UPROPERTY()
	bool bHoldingTrigger = false;

	EHandPose EvaluateHandPose() const;
};

/// A MotionController's region history and hand pose history, along with it's current HandPose
/// 
USTRUCT()
struct FControllerMinimalData
{
	GENERATED_BODY()
	
	UPROPERTY()
	FControllerMinimalStateHistory HorizonalStateHistory{}; // 8 bytes

	UPROPERTY()
	FControllerMinimalStateHistory VerticalStateHistory{}; // 8 bytes

	UPROPERTY()
	EHandPose HandPose = EHandPose::OpenHand;  // 1 byte

	UPROPERTY()
	FControllerKeysState KeysState{}; // 2 bytes todo
	
	void TriggerKeyEvent(EControllerKeyEvent KeyEvent);
};


/// Used to mark the tilt level in a state, or accepted tilt levels of the controller in a defined motion
///
UENUM(BlueprintType, meta = (BitFlags))
enum class EControllerTilt : uint8
{
	Ignore = 0,
	PalmUp = BIT(1),
	PalmDown = BIT(2),
	PalmInwards = BIT(3),
	PalmOutwards = BIT(4),
};
ENUM_CLASS_FLAGS(EControllerTilt)

/// This is controller state data that only needs to be checked for and updated if the region history gets updated.
///		The arrays are 8 items since each item is represented by an FMotionState.
///		A FMotionState is an uint8, a chain of 8 FMotionStates is 8 * uint8s = a uint64 = 8 bytes => 8 array entries).
USTRUCT()
struct FControllerFatData
{
	GENERATED_BODY()

	UPROPERTY()
	FVector2D DirectionToControllerXY = FVector2D(1.f, 0.f); // (from HMD)
	
	UPROPERTY()
	uint8 CurrentIndex = 0;
	
	UPROPERTY()
	FVector ControllerWorldLocationHistory[8];
	
	UPROPERTY()
	float RadiansTurnedAroundPlayerHistory[8] = {0}; // a relative measurement of how many radians the controller has turned since the previous state

	UPROPERTY()
	uint64 ControllerTiltHistory = 0;
	
	
	void UpdateCurrentIndex();
	void Update(const FVector& ControllerLocation, float RadiansTurned, EControllerTilt ControllerTilt);

	/// When updating the data, the data is put into the arrays in the index order 7->8->0->1, etc (looping).
	///		This function returns them in the order 0->1->2.., where the last item is the latest.
	void GetControllerHistoryDataInOrder(float (& OutRadiansTurnedAroundPlayerHistory)[8], FVector (& OutControllerWorldLocationHistory)[8]) const;
};



USTRUCT(BlueprintType)
struct FMotionRegionBounds
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Horizontal regions")
	float InnerBound = 25.f;
	
	UPROPERTY(EditAnywhere, Category = "Horizontal regions")
	float OuterBound = 60.f;

	UPROPERTY(EditAnywhere, Category = "Vertical regions")
	float LowerBound = -30.f;
	
	UPROPERTY(EditAnywhere, Category = "Vertical regions")
	float UpperBound = -70.f;

	void FindRegions(EHorizRegion& HorizontalRegion, EVertRegion& VerticalRegion, const FVector ControllerOffset) const;
private:
	/// Finds the region for a Controller based on it's offset from the HMD as well as the input delimiters.
	EHorizRegion FindHorizontalRegion(const FVector& ControllerOffset) const;
	EVertRegion FindVerticalRegion(const FVector& ControllerOffset) const;
};



namespace MotionState
{
	constexpr uint8 AsU8(EHorizRegion Region) { return static_cast<uint8>(Region); }
	constexpr uint8 AsU8(EVertRegion Region) { return static_cast<uint8>(Region); }
	constexpr uint8 AsU8(EHandPose HandPose) { return static_cast<uint8>(HandPose); }
	
	constexpr uint8 RegionMask = 7; // 0000 0111
	constexpr uint8 HandPoseMask = 56; // 0011 1000

	FORCEINLINE static uint64 LastStatesMask(uint8 StateCount)
	{
		check(StateCount <= 8);
		//TEXT("RegionCount %u must be <= 8, or it's going to be over the u64 limit"), RegionCount);
		uint64 Mask = 0;
		for (int i = 0; i < StateCount; i++)
		{
			Mask <<= 8;
			Mask |= 0xff;
		}
		return Mask;
	}

	// returns the last X states in the chain
	FORCEINLINE static uint64 FilterLastStates(uint64 StateChain, uint8 RegionsCount) { return StateChain & LastStatesMask(RegionsCount); }
}


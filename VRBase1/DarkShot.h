// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DarkShot.generated.h"

/// Base class for projectiles fired from motions
UCLASS()
class VRBASE1_API ADarkShot : public AActor
{
	GENERATED_BODY()

public:
	ADarkShot();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};

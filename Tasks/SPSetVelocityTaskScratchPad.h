// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tasks/IAbleAbilityTask.h"
#include "SPSetVelocityTaskScratchPad.generated.h"

/**
 * 
 */
UCLASS()
class FEATURE_SP_API USPSetVelocityTaskScratchPad : public UAbleAbilityTaskScratchPad
{
	GENERATED_BODY()

public:
	bool bIsSetVelocity = false;

	FVector VelocityValue = FVector::ZeroVector;

	double StartTime = 0.0f; 
};

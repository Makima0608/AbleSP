// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tasks/IAbleAbilityTask.h"
#include "SPSetBoneCapsuleTaskScratchPad.generated.h"

/**
 * 
 */
UCLASS()
class FEATURE_SP_API USPSetBoneCapsuleTaskScratchPad : public UAbleAbilityTaskScratchPad
{
	GENERATED_BODY()
public:
	FVector PreScale = FVector::ZeroVector;
};

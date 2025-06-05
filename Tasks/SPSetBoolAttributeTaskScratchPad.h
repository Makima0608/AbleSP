
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tasks/IAbleAbilityTask.h"
#include "SPSetBoolAttributeTaskScratchPad.generated.h"


UCLASS(Transient)
class FEATURE_SP_API USPSetBoolAttributeTaskScratchPad : public UAbleAbilityTaskScratchPad
{
	GENERATED_BODY()
public:
	bool m_OriginalValue = false;
	
	USPSetBoolAttributeTaskScratchPad();
	virtual ~USPSetBoolAttributeTaskScratchPad();
};




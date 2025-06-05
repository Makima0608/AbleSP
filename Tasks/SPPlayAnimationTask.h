// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tasks/ablePlayAnimationTask.h"
#include "SPPlayAnimationTask.generated.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

/**
 * 
 */
UCLASS()
class FEATURE_SP_API USPPlayAnimationTask : public UAblePlayAnimationTask
{
	GENERATED_BODY()

protected:
	virtual FString GetModuleName_Implementation() const override;

public:
#if WITH_EDITOR
	virtual FText GetTaskName() const override { return LOCTEXT("SPPlayAnimationTask", "SP Play Animation"); }

#endif

protected:
	virtual UAnimMontage* PlayMontageBySequence(UAnimInstance* Instance, UAnimSequenceBase* Asset, FName SlotNodeName, float BlendInTime = 0.25f, float BlendOutTime = 0.25f, float InPlayRate = 1.f, int32 LoopCount = 1, float BlendOutTriggerTime = -1.f, float InTimeToStartMontageAt = 0.f, bool bStopAllMontages = true, float Weight = 1.0f) const override;
};

#undef LOCTEXT_NAMESPACE
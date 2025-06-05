// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UnLuaInterface.h"
#include "Tasks/ablePlayAnimationTask.h"

#include "SPCorrectedPlayAnimTask.generated.h"



#define LOCTEXT_NAMESPACE "SPAbleAbilityTask"

/*
*  PlayAnimationTaskPlay Rate
*/
UCLASS(BlueprintType)
class FEATURE_SP_API USPCorrectedPlayAnimTask : public UAblePlayAnimationTask
{
	GENERATED_BODY()

protected:
	virtual FString GetModuleName_Implementation() const override;

// ~UAbleAbilityTask begin
protected:
	virtual bool IsDone(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override;
	virtual bool IsDoneBP_Implementation(const UAbleAbilityContext* Context) const override;

#if WITH_EDITOR
	virtual FText GetTaskCategory() const override { return LOCTEXT("SPCorrectedPlayAnimTaskCategory", "Animation"); }
	virtual FText GetTaskName() const override { return LOCTEXT("SPCorrectedPlayAnimTask", "Corrected Play Animation"); }
	virtual FText GetTaskDescription() const override { return LOCTEXT("SPCorrectedPlayAnimTaskDesc", "Corrected Play Animation"); }
#endif
// ~UAbleAbilityTask end

// ~UAblePlayAnimationTask begin
protected:
	virtual void OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const override;
// ~UAblePlayAnimationTask end

public:
	USPCorrectedPlayAnimTask(const FObjectInitializer& ObjectInitializer);
};

#undef LOCTEXT_NAMESPACE

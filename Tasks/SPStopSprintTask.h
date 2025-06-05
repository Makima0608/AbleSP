// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tasks/IAbleAbilityTask.h"
#include "SPStopSprintTask.generated.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

/**
 * 
 */
UCLASS()
class FEATURE_SP_API USPStopSprintTask : public UAbleAbilityTask
{
	GENERATED_BODY()

public:
	virtual void OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskStart"))
	void OnTaskStartBP(const UAbleAbilityContext* Context) const;
	
	virtual void OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context, const EAbleAbilityTaskResult result) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskEnd"))
	void OnTaskEndBP(const UAbleAbilityContext* Context,
					 const EAbleAbilityTaskResult result) const;

	virtual TStatId GetStatId() const override;

#if WITH_EDITOR

	virtual FText GetTaskCategory() const override { return LOCTEXT("USPStopSprintTask", "GamePlay"); }

	virtual FText GetTaskName() const override { return LOCTEXT("USPStopSprintTask", "Stop Sprint"); }

	virtual bool CanEditTaskRealm() const override { return true; }
#endif
};

#undef LOCTEXT_NAMESPACE
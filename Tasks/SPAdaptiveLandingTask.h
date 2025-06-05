// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tasks/IAbleAbilityTask.h"
#include "SPAdaptiveLandingTask.generated.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

UCLASS(Transient)
class USPAdaptiveLandingTaskScratchPad : public UAbleAbilityTaskScratchPad
{
	GENERATED_BODY()

public:
	USPAdaptiveLandingTaskScratchPad();

	virtual ~USPAdaptiveLandingTaskScratchPad();

	UPROPERTY()
	float DuringTime = 0;
};

/**
 * 
 */
UCLASS()
class FEATURE_SP_API USPAdaptiveLandingTask : public UAbleAbilityTask
{
	GENERATED_BODY()
	
private:
	void MoveActorToAdaptLand(const TWeakObjectPtr<const UAbleAbilityContext>& Context, float deltaTime) const;
	
public:
	virtual void OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskStart"))
	void OnTaskStartBP(const UAbleAbilityContext* Context) const;

	virtual void OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
						   const EAbleAbilityTaskResult result) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskEnd"))
	void OnTaskEndBP(const UAbleAbilityContext* Context,
					 const EAbleAbilityTaskResult result) const;

	virtual void OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context, float deltaTime) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskStart"))
	void OnTaskTickBP(const UAbleAbilityContext* Context, float deltaTime) const;

	virtual TStatId GetStatId() const override;

	virtual EAbleAbilityTaskRealm GetTaskRealm() const override;

	virtual UAbleAbilityTaskScratchPad*
	CreateScratchPad(const TWeakObjectPtr<UAbleAbilityContext>& Context) const override;


#if WITH_EDITOR

	virtual FText GetTaskCategory() const override { return LOCTEXT("USPAdaptiveLandingTask", "GamePlay"); }

	virtual FText GetTaskName() const override { return LOCTEXT("USPAdaptiveLandingTask", "Adaptive Landing"); }

	virtual bool CanEditTaskRealm() const override { return true; }
#endif

protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, meta = (DisplayName = "MaxAdaptDistance"))
	float MaxAdaptDistance = 300.0f;
};

#undef LOCTEXT_NAMESPACE


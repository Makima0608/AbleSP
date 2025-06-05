// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tasks/IAbleAbilityTask.h"
#include "SPPauseFrameTask.generated.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

UCLASS(Transient)
class USPPauseFrameTaskScratchPad : public UAbleAbilityTaskScratchPad
{
	GENERATED_BODY()

public:
	USPPauseFrameTaskScratchPad();

	virtual ~USPPauseFrameTaskScratchPad();

	UPROPERTY()
	float DuringTime = 0;

	UPROPERTY()
	bool bIsDuringPause = false;

	UPROPERTY()
	float PauseStartTime = 0.0f;

	UPROPERTY()
	float MontagePauseTime = 0.0f;

	UPROPERTY()
	int HasPauseIndex = 0;
};

/**
 * 
 */
UCLASS()
class FEATURE_SP_API USPPauseFrameTask : public UAbleAbilityTask
{
	GENERATED_BODY()
	
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

	virtual FText GetTaskCategory() const override { return LOCTEXT("USPPauseFrameTask", "GamePlay"); }

	virtual FText GetTaskName() const override { return LOCTEXT("USPPauseFrameTask", "Pause Frame"); }

	virtual bool CanEditTaskRealm() const override { return true; }
#endif

protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, meta = (DisplayName = "StartPauseTimeList"))
	TArray<float> StartPauseTimeList;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, meta = (DisplayName = "PauseDuringTime"))
	float PauseDuringTime = 0.0f;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, meta = (DisplayName = "bIsOpenDebugPoint"))
	bool bIsOpenDebugPoint = false;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, meta = (DisplayName = "bIsJumpOver"))
	bool bIsJumpOver = false;
};

#undef LOCTEXT_NAMESPACE
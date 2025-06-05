// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tasks/IAbleAbilityTask.h"
#include "SPAdjustRealTimeAnimMoveRatioTask.generated.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

UCLASS(Transient)
class USPAdjustRealTimeAnimMoveRatioTaskScratchPad : public UAbleAbilityTaskScratchPad
{
	GENERATED_BODY()

public:
	USPAdjustRealTimeAnimMoveRatioTaskScratchPad();

	virtual ~USPAdjustRealTimeAnimMoveRatioTaskScratchPad();

	UPROPERTY()
	float DuringTime = 0;

	UPROPERTY()
	float CacheAnimMoveRatio = 1.0f;
};

/**
 * 
 */
UCLASS()
class FEATURE_SP_API USPAdjustRealTimeAnimMoveRatioTask : public UAbleAbilityTask
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

	virtual void OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context, float deltaTime) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskStart"))
	void OnTaskTickBP(const UAbleAbilityContext* Context, float deltaTime) const;

	virtual TStatId GetStatId() const override;

	virtual UAbleAbilityTaskScratchPad* CreateScratchPad(const TWeakObjectPtr<UAbleAbilityContext>& Context) const override;


#if WITH_EDITOR

	virtual FText GetTaskCategory() const override { return LOCTEXT("USPAdjustRealTimeAnimMoveRatioTask", "GamePlay"); }

	virtual FText GetTaskName() const override { return LOCTEXT("USPAdjustRealTimeAnimMoveRatioTask", "Adjust RealTime AnimMoveRatio"); }

	virtual bool CanEditTaskRealm() const override { return true; }
#endif

protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, meta = (DisplayName = "AnimMoveRatioCurve"))
	TMap<FName, UCurveFloat*> AnimMoveRatioCurve;
};

#undef LOCTEXT_NAMESPACE
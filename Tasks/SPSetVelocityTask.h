// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tasks/IAbleAbilityTask.h"
#include "SPSetVelocityTask.generated.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"
/**
 * 
 */
UCLASS()
class FEATURE_SP_API USPSetVelocityTask : public UAbleAbilityTask
{
	GENERATED_BODY()

private:
	FVector VelocityValue = FVector::ZeroVector;

	bool bIsSetVelocity = false;

	void MoveActorByVelocity(const TWeakObjectPtr<const UAbleAbilityContext>& Context, float deltaTime) const;
public:
	virtual TStatId GetStatId() const override;
	virtual void OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override;

	virtual void OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context, float deltaTime) const override;

	virtual void OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context, const EAbleAbilityTaskResult result) const override;
	
	virtual UAbleAbilityTaskScratchPad* CreateScratchPad(const TWeakObjectPtr<UAbleAbilityContext>& Context) const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsSetVelocityToContext = false;

#if WITH_EDITOR
	virtual FText GetTaskName() const { return LOCTEXT("SPSetVelocityTask", "SP SetVelocity Task"); }

#endif

	virtual EAbleAbilityTaskRealm GetTaskRealm() const override { return GetTaskRealmBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "GetTaskRealm"))
	EAbleAbilityTaskRealm GetTaskRealmBP() const;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float ClientSetVelocityDuringTime = 0.1f;
};

#undef LOCTEXT_NAMESPACE

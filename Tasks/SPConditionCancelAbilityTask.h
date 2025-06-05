// Copyright (c) Extra Life Studios, LLC. All rights reserved.

#pragma once

#include "UnLuaInterface.h"
#include "CoreMinimal.h"
#include "Tasks/IAbleAbilityTask.h"
#include "Tasks/ableBranchCondition.h"
#include "Tasks/ableCancelAbilityTask.h"
#include "SPConditionCancelAbilityTask.generated.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

/**
 * 
 */
UCLASS()
class FEATURE_SP_API USPConditionCancelAbilityTask : public UAbleAbilityTask, public IUnLuaInterface
{
	GENERATED_BODY()

protected:
	virtual FString GetModuleName_Implementation() const override;

public:
	USPConditionCancelAbilityTask(const FObjectInitializer& ObjectInitializer);

	~USPConditionCancelAbilityTask();

	virtual void OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskStart"))
	void OnTaskStartBP(const UAbleAbilityContext* Context) const;

	virtual void OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	                       const EAbleAbilityTaskResult result) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskEnd"))
	void OnTaskEndBP(const UAbleAbilityContext* Context,
					 const EAbleAbilityTaskResult result) const;

	virtual TStatId GetStatId() const override;

	bool CheckBranchCondition(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const;

	void InternalDoCancel(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const;

	bool ShouldCancelAbility(const UAbleAbility& Ability, const UAbleAbilityContext& Context) const;

	virtual bool IsSingleFrame() const override { return IsSingleFrameBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "IsSingleFrame"))
	bool IsSingleFrameBP() const;

	virtual EAbleAbilityTaskRealm GetTaskRealm() const override { return GetTaskRealmBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "GetTaskRealm"))
	EAbleAbilityTaskRealm GetTaskRealmBP() const;

#if WITH_EDITOR

	virtual FText GetTaskCategory() const override { return LOCTEXT("USPConditionCancelAbilityTask", "Logic"); }

	virtual FText GetTaskName() const override
	{
		return LOCTEXT("USPConditionCancelAbilityTask", "Condition Cancel Ability");
	}

#endif

protected:
	UPROPERTY(EditAnywhere, Instanced, Category = "Condition Cancel Ability", meta = (DisplayName = "Conditions"))
	TArray<UAbleBranchCondition*> m_CancelConditions;

	UPROPERTY(EditAnywhere, Category = "Condition Cancel Ability", meta = (DisplayName = "Must Pass All Conditions"))
	bool m_MustPassAllCancelConditions;

	UPROPERTY(EditAnywhere, Category = "Condition Cancel Ability", meta = (DisplayName = "Passive Behavior"))
	TEnumAsByte<EAbleCancelAbilityPassiveBehavior> m_PassiveBehavior;

	/* What result to pass Abilities that are canceled */
	UPROPERTY(EditAnywhere, Category = "Condition Cancel Ability", meta = (DisplayName = "Cancel Result"))
	TEnumAsByte<EAbleAbilityTaskResult> m_CancelResult;
};

#undef LOCTEXT_NAMESPACE

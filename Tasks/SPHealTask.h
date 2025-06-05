// Copyright (c) Extra Life Studios, LLC. All rights reserved.

#pragma once

#include "UnLuaInterface.h"
#include "CoreMinimal.h"
#include "Tasks/IAbleAbilityTask.h"
#include "SPHealTask.generated.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

/**
 * 
 */
UCLASS()
class FEATURE_SP_API USPHealTask : public UAbleAbilityTask, public IUnLuaInterface
{
	GENERATED_BODY()

protected:
	virtual FString GetModuleName_Implementation() const override;

public:
	USPHealTask(const FObjectInitializer& ObjectInitializer);
	
	~USPHealTask();
	
	virtual void OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskStart"))
	void OnTaskStartBP(const UAbleAbilityContext* Context) const;
	
	virtual void OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	                       const EAbleAbilityTaskResult result) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskEnd"))
	void OnTaskEndBP(const UAbleAbilityContext* Context,
					 const EAbleAbilityTaskResult result) const;
	
	virtual bool IsSingleFrame() const override { return IsSingleFrameBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "IsSingleFrame"))
	bool IsSingleFrameBP() const;

	virtual EAbleAbilityTaskRealm GetTaskRealm() const override { return GetTaskRealmBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "GetTaskRealm"))
	EAbleAbilityTaskRealm GetTaskRealmBP() const;

	virtual TStatId GetStatId() const override;

#if WITH_EDITOR
	
	virtual FText GetTaskName() const override { return LOCTEXT("USPHealTask", "Heal"); }

	virtual FText GetTaskCategory() const override { return LOCTEXT("USPHealTask", "Damage"); }
	
	virtual bool CanEditTaskRealm() const override { return true; }
#endif
	
protected:
	void GetHealTargets(const TWeakObjectPtr<const UAbleAbilityContext>& Context, TArray<TWeakObjectPtr<AActor>>& OutArray) const;
	
	UPROPERTY(EditAnywhere, Category = "Heal", meta = (DisplayName = "Heal Amount"))
	float m_HealValue;

	UPROPERTY(EditAnywhere, Category = "Heal", meta = (DisplayName = "Heal Source"))
	TEnumAsByte<EAbleAbilityTargetType> m_HealSource;
	
	UPROPERTY(EditAnywhere, Category = "Heal", meta = (DisplayName = "Heal Targets"))
	TArray<TEnumAsByte<EAbleAbilityTargetType>> m_HealTargets;
};

#undef LOCTEXT_NAMESPACE
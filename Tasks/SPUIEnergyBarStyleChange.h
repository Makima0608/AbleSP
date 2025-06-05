// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UnLuaInterface.h"
#include "CoreMinimal.h"
#include "Game/SPGame/Utils/SPGameLibrary.h"
#include "Tasks/IAbleAbilityTask.h"
#include "SPUIEnergyBarStyleChange.generated.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

/**
 * 
 */
UCLASS()
class FEATURE_SP_API USPUIEnergyBarStyleChange : public UAbleAbilityTask, public IUnLuaInterface
{
	GENERATED_BODY()

protected:
	virtual FString GetModuleName_Implementation() const override;

public:
#if WITH_EDITOR
	virtual FText GetTaskCategory() const override { return LOCTEXT("SPUIEnergyBarStyleChange", "Player"); }

	virtual FText GetTaskName() const override { return LOCTEXT("SPUIEnergyBarStyleChange", "SP UI Energy Bar Style Change Task"); }
#endif

	virtual TStatId GetStatId() const override;
	
	virtual EAbleAbilityTaskRealm GetTaskRealm() const override { return GetTaskRealmBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "GetTaskRealm"))
	EAbleAbilityTaskRealm GetTaskRealmBP() const;
	
	virtual void OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskStart"))
	void OnTaskStartBP(const UAbleAbilityContext* Context) const;

	virtual void OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context, const EAbleAbilityTaskResult result) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskEnd"))
	void OnTaskEndBP(const UAbleAbilityContext* Context, const EAbleAbilityTaskResult result) const;

	UPROPERTY(EditAnywhere, Category = "StyleType", meta=(DisplayName="能量条风格类型"))
	int m_StyleType; // 考虑后面换成枚举 - 0 默认 1 特殊
};

#undef LOCTEXT_NAMESPACE

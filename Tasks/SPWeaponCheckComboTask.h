// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UnLuaInterface.h"
#include "CoreMinimal.h"
#include "Tasks/IAbleAbilityTask.h"
#include "SPWeaponCheckComboTask.generated.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

class ASPComboMeleeWeapon;
class USPGameCharInventoryComponent;

/**
 *
 */

UENUM(BlueprintType)
enum class ESPComboActiveMode : uint8
{
	Immediately UMETA(DisplayName="立即跳转"),
	AtEnd UMETA(DisplayName="结尾时跳转"),
};

UENUM(BlueprintType)
enum class ESPComboIndexMode : uint8
{
	Next UMETA(DisplayName="下一段"),
	Prev UMETA(DisplayName="上一段"),
	Index UMETA(DisplayName="指定段"),
	First UMETA(DisplayName="第一段"),
	Last UMETA(DisplayName="最后一段"),
};

UENUM(BlueprintType)
enum class ESPComboInputMode : uint8
{
	Pressed UMETA(DisplayName="按下"),
	Hold UMETA(DisplayName="长按"),
	Auto UMETA(DisplayName="自动触发"),
};

/* Scratchpad for our Task. */
UCLASS(Transient)
class USPWeaponCheckComboTaskScratchPad : public UAbleAbilityTaskScratchPad
{
	GENERATED_BODY()
public:
	USPWeaponCheckComboTaskScratchPad();
	virtual ~USPWeaponCheckComboTaskScratchPad() override;
	
	UPROPERTY(Transient)
	bool bComboTriggered = false;

	UPROPERTY(Transient)
	bool bPressedOnStart = false;

	UPROPERTY(Transient)
	float CurrentHoldTime = 0.f;

	UPROPERTY(Transient)
	TWeakObjectPtr<ASPComboMeleeWeapon> ComboWeapon;
	
	UPROPERTY(Transient)
	TWeakObjectPtr<USPGameCharInventoryComponent> SPInventoryComp;
};

UCLASS()
class FEATURE_SP_API USPWeaponCheckComboTask : public UAbleAbilityTask, public IUnLuaInterface
{
	GENERATED_BODY()

protected:
	virtual FString GetModuleName_Implementation() const override;

public:
	USPWeaponCheckComboTask(const FObjectInitializer& ObjectInitializer);
	virtual ~USPWeaponCheckComboTask() override;

	virtual void OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskStart"))
	void OnTaskStartBP(const UAbleAbilityContext* Context) const;
	
	virtual void OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context, float deltaTime) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskTick"))
	void OnTaskTickBP(const UAbleAbilityContext* Context, float deltaTime) const;

	virtual void OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context, const EAbleAbilityTaskResult result) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskEnd"))
	void OnTaskEndBP(const UAbleAbilityContext* Context, const EAbleAbilityTaskResult result) const;

	virtual bool IsSingleFrame() const override { return IsSingleFrameBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "IsSingleFrame"))
	bool IsSingleFrameBP() const;

	virtual TStatId GetStatId() const override;

	virtual EAbleAbilityTaskRealm GetTaskRealm() const override { return GetTaskRealmBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "GetTaskRealm"))
	EAbleAbilityTaskRealm GetTaskRealmBP() const;

	virtual UAbleAbilityTaskScratchPad* CreateScratchPad(const TWeakObjectPtr<UAbleAbilityContext>& Context) const override;

	virtual void Serialize(FArchive& Ar) override;

#if WITH_EDITOR

	virtual FText GetTaskCategory() const override { return LOCTEXT("SPWeaponCheckComboTask", "Weapon"); }

	virtual FText GetTaskName() const override { return LOCTEXT("USPWeaponCheckComboTask", "Check Combo"); }

	virtual bool CanEditTaskRealm() const override { return true; }
#endif

protected:
	bool ActivateCombo(ASPComboMeleeWeapon* Weapon) const;
	int32 GetComboIndex(const ASPComboMeleeWeapon* Weapon) const;

public:
	UPROPERTY(EditAnywhere, Category = "Weapon", meta=(DisplayName="Reset Combo", DeprecatedProperty, DeprecationMessage = "字段废弃，使用“连击索引类型”指定是否重置连击"))
	bool bResetCombo = false;
	
	UPROPERTY(EditAnywhere, Category = "Weapon", meta=(DisplayName="连击激活类型", EditCondition = "InputMode != ESPComboInputMode::Auto", EditConditionHides))
	ESPComboActiveMode ActivateMode = ESPComboActiveMode::Immediately;

	UPROPERTY(EditAnywhere, Category = "Weapon", meta=(DisplayName="连击索引类型"))
	ESPComboIndexMode IndexMode = ESPComboIndexMode::Next;	

	UPROPERTY(EditAnywhere, Category = "Weapon", meta=(DisplayName="连击索引", UIMin = 0, ClampMin = 0, EditCondition = "IndexMode == ESPComboIndexMode::Index", EditConditionHides))
	int32 ComboIndex = 0;
	
	UPROPERTY(EditAnywhere, Category = "Weapon", meta=(DisplayName="输入模式"))
	ESPComboInputMode InputMode = ESPComboInputMode::Pressed;

	UPROPERTY(EditAnywhere, Category = "Weapon", meta=(DisplayName="时长", EditCondition = "InputMode == ESPComboInputMode::OnHold", EditConditionHides))
	float HoldTime = 0.2f;
};

#undef LOCTEXT_NAMESPACE

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UnLuaInterface.h"
#include "CoreMinimal.h"
#include "Tasks/IAbleAbilityTask.h"
#include "SPWeaponShowHideTask.generated.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"
/**
 * 
 */
/* Scratchpad for our Task. */
UCLASS(Transient)
class USPWeaponShowHideTaskScratchPad : public UAbleAbilityTaskScratchPad
{
	GENERATED_BODY()
public:
	USPWeaponShowHideTaskScratchPad();
	virtual ~USPWeaponShowHideTaskScratchPad();

	/* Any turns in progress. */
	UPROPERTY(transient)
	bool OriginWeaponVisibility;
};

UCLASS()
class FEATURE_SP_API USPWeaponShowHideTask : public UAbleAbilityTask, public IUnLuaInterface
{
	GENERATED_BODY()

protected:
	virtual FString GetModuleName_Implementation() const override;

public:
	
	USPWeaponShowHideTask(const FObjectInitializer& ObjectInitializer);

	virtual void OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskStart"))
	void OnTaskStartBP(const UAbleAbilityContext* Context) const;
	
	/* End our Task. */
	virtual void OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context, const EAbleAbilityTaskResult result) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskEnd"))
	void OnTaskEndBP(const UAbleAbilityContext* Context, const EAbleAbilityTaskResult Result) const;

	virtual TStatId GetStatId() const override;

	/* Returns the realm our Task belongs to. */
	virtual EAbleAbilityTaskRealm GetTaskRealm() const override { return GetTaskRealmBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "GetTaskRealm"))
	EAbleAbilityTaskRealm GetTaskRealmBP() const;
	
	virtual UAbleAbilityTaskScratchPad* CreateScratchPad(const TWeakObjectPtr<UAbleAbilityContext>& Context) const override;

	virtual bool IsSingleFrame() const override { return IsSingleFrameBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "IsSingleFrame"))
	bool IsSingleFrameBP() const;

#if WITH_EDITOR
	
	virtual FText GetTaskName() const override { return LOCTEXT("USPWeaponShowHideTask", "WeaponShowHide"); }

	virtual FText GetTaskCategory() const override { return LOCTEXT("USPWeaponShowHideTask", "WeaponShowHide"); }
	
	virtual bool CanEditTaskRealm() const override { return true; }
#endif

	UPROPERTY(EditAnywhere,meta=(DisplayName = "隐藏武器"))
	bool HideWeapon;

protected:
	UPROPERTY(EditAnywhere, meta=(ToolTip = "单帧"))
	bool bIsSingleFrame = false;

	UPROPERTY(EditAnywhere, meta=(ToolTip = "结束时重置"))
	bool bResetAtEnd = true;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (DisplayName = "Target", ToolTip = "查找执行的目标类型"))
	TEnumAsByte<EAbleAbilityTargetType> m_Target;
};

#undef LOCTEXT_NAMESPACE
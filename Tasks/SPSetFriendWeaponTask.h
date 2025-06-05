// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UnLuaInterface.h"
#include "CoreMinimal.h"
#include "Tasks/IAbleAbilityTask.h"
#include "SPSetFriendWeaponTask.generated.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"
/**
 * 
 */
UCLASS()
class FEATURE_SP_API USPSetFriendWeaponTask : public UAbleAbilityTask, public IUnLuaInterface
{
	GENERATED_BODY()

protected:
	virtual FString GetModuleName_Implementation() const override;

public:

	USPSetFriendWeaponTask(const FObjectInitializer& ObjectInitializer);
	
	virtual ~USPSetFriendWeaponTask() override;

	virtual void OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskStart"))
	void OnTaskStartBP(const UAbleAbilityContext* Context) const;
	
	virtual void OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context, const EAbleAbilityTaskResult result) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskEnd"))
	void OnTaskEndBP(const UAbleAbilityContext* Context, const EAbleAbilityTaskResult result) const;

	virtual bool IsSingleFrame() const override { return IsSingleFrameBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "IsSingleFrame"))
	bool IsSingleFrameBP() const;

	virtual void BindDynamicDelegates(UAbleAbility* Ability) override;

	virtual TStatId GetStatId() const override;

	virtual EAbleAbilityTaskRealm GetTaskRealm() const override { return GetTaskRealmBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "GetTaskRealm"))
	EAbleAbilityTaskRealm GetTaskRealmBP() const;

#if WITH_EDITOR

	virtual FText GetTaskCategory() const override { return LOCTEXT("SPSetFriendWeaponTask", "Weapon"); }

	virtual FText GetTaskName() const override { return LOCTEXT("USPSetWeaponTask", "Set FriendWeapon"); }

	virtual bool CanEditTaskRealm() const override { return true; }
#endif

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (DisplayName = "目标"))
	TEnumAsByte<EAbleAbilityTargetType> m_Target = EAbleAbilityTargetType::ATT_Self;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (DisplayName = "是否装配"))
	bool m_bEquip;
};

#undef LOCTEXT_NAMESPACE
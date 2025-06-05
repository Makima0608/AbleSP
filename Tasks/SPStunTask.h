// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UnLuaInterface.h"
#include "CoreMinimal.h"
#include "Tasks/IAbleAbilityTask.h"
#include "SPStunTask.generated.h"


#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"
/**
 * 
 */
UCLASS()
class FEATURE_SP_API USPStunTask : public UAbleAbilityTask, public IUnLuaInterface
{
	GENERATED_BODY()

protected:
	virtual FString GetModuleName_Implementation() const override;

public:

	USPStunTask(const FObjectInitializer& ObjectInitializer);
	virtual ~USPStunTask();
	
	virtual void OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskStart"))
	void OnTaskStartBP(const UAbleAbilityContext* Context) const;

	virtual void OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,const EAbleAbilityTaskResult result) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskEnd"))
	void OnTaskEndBP(const UAbleAbilityContext* Context,const EAbleAbilityTaskResult result) const;

	virtual bool IsSingleFrame() const override { return IsSingleFrameBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "IsSingleFrame"))
	bool IsSingleFrameBP() const;

	virtual bool IsAsyncFriendly() const override { return false; }

	virtual void BindDynamicDelegates(UAbleAbility* Ability) override;

	virtual EAbleAbilityTaskRealm GetTaskRealm() const override { return GetTaskRealmBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "GetTaskRealm"))
	EAbleAbilityTaskRealm GetTaskRealmBP() const;

	virtual TStatId GetStatId() const override;

	UPROPERTY(EditAnywhere, Category = "StunTask",meta = (DisplayName = "StunTaskTime", AbleBindableProperty))
	float StunTaskTime = 6;

#if WITH_EDITOR

	virtual FText GetTaskCategory() const override { return LOCTEXT("USPStunTask", "Stun"); }

	virtual FText GetTaskName() const override { return LOCTEXT("USPStunTask", "Stun"); }

#endif
};

#undef LOCTEXT_NAMESPACE



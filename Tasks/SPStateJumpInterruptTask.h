// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UnLuaInterface.h"
#include "CoreMinimal.h"
#include "Game/SPGame/SPGameDefine.h"
#include "MoeGameplay/SceneElements/Others/MoeConveyorCommonBase.h"
#include "Tasks/IAbleAbilityTask.h"
#include "SPStateJumpInterruptTask.generated.h"


#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

/**
 * 
 */
UCLASS()
class FEATURE_SP_API USPStateJumpInterruptTask : public UAbleAbilityTask, public IUnLuaInterface
{
	GENERATED_BODY()

protected:
	virtual FString GetModuleName_Implementation() const override;

public:
#if WITH_EDITOR
	virtual FText GetTaskCategory() const override { return LOCTEXT("SPStateJumpChooseInterruptTask", "Player"); }

	//仅仅选几个可以打断该状态的状态
	virtual FText GetTaskName() const override { return LOCTEXT("SPStateJumpChooseInterruptTask", "State Jump ChooseInterrupt Task"); }
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

	UPROPERTY(EditAnywhere, Category = "StateBlock", meta=(DisplayName="ActionStateBlockInterrupt(弃用、仅对照)"))
	TArray<ECharActionState> InterruptActionState;

	UPROPERTY(EditAnywhere, Category = "StateBlock", meta=(DisplayName="ActionStateBlockInterrupt_Main"))
	TArray<ECharActionState> InterruptActionState_Main;

	UPROPERTY(EditAnywhere, Category = "StateBlock", meta=(DisplayName="ActionStateBlockInterrupt_SP"))
	TArray<ESPActionState> InterruptActionState_SP;

	UPROPERTY(EditAnywhere, Category = "StateBlock", meta=(DisplayName="MotionStateBlockInterrupt(弃用、仅对照)"))
	TArray<ECharMotionState> InterruptMotionState;

	UPROPERTY(EditAnywhere, Category = "StateBlock", meta=(DisplayName="MotionStateBlockInterrupt_Main"))
	TArray<ECharMotionState> InterruptMotionState_Main;

	UPROPERTY(EditAnywhere, Category = "StateBlock", meta=(DisplayName="MotionStateBlockInterrupt_SP"))
	TArray<ESPMotionState> InterruptMotionState_SP;

	UPROPERTY(EditAnywhere, Category = "StateBlock", meta=(DisplayName="bShouldInActionState"))
	bool bShouldInActionState = false;

	UPROPERTY(EditAnywhere, Category = "StateBlock", meta=(DisplayName="bShouldInMotionState"))
	bool bShouldInMotionState = false;

	UPROPERTY(EditAnywhere, Category = "StateBlock", meta=(DisplayName="CorrectActionState(弃用、仅对照)"))
	ECharActionState CorrectActionState = ECharActionState::None;

	UPROPERTY(EditAnywhere, Category = "StateBlock", meta=(DisplayName="CorrectActionState_Main"))
	ECharActionState CorrectActionState_Main = ECharActionState::None;

	UPROPERTY(EditAnywhere, Category = "StateBlock", meta=(DisplayName="CorrectActionState_SP"))
	ESPActionState CorrectActionState_SP = ESPActionState::SP_Begin;
	
	UPROPERTY(EditAnywhere, Category = "StateBlock", meta=(DisplayName="CorrectMotionState(弃用、仅对照)"))
	ECharMotionState CorrectMotionState = ECharMotionState::Default;

	UPROPERTY(EditAnywhere, Category = "StateBlock", meta=(DisplayName="CorrectMotionState_Main"))
	ECharMotionState CorrectMotionState_Main = ECharMotionState::Default;

	UPROPERTY(EditAnywhere, Category = "StateBlock", meta=(DisplayName="CorrectMotionState_SP"))
	ESPMotionState CorrectMotionState_SP = ESPMotionState::None;
};

#undef LOCTEXT_NAMESPACE
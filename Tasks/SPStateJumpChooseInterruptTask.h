// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UnLuaInterface.h"
#include "CoreMinimal.h"
#include "Game/SPGame/SPGameDefine.h"
#include "MoeGameplay/SceneElements/Others/MoeConveyorCommonBase.h"
#include "Tasks/IAbleAbilityTask.h"
#include "SPStateJumpChooseInterruptTask.generated.h"


#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

/**
 * 
 */
UCLASS()
class FEATURE_SP_API USPStateJumpChooseInterruptTask : public UAbleAbilityTask, public IUnLuaInterface
{
	GENERATED_BODY()

protected:
	virtual FString GetModuleName_Implementation() const override;

public:
#if WITH_EDITOR
	virtual FText GetTaskCategory() const override { return LOCTEXT("SPStateJumpChooseInterruptTask", "Player"); }

	//不想让他被任何其他状态打断，给一个默认不打断的计数加一，然后放出唯N个可以打断的状态
	virtual FText GetTaskName() const override { return LOCTEXT("SPStateJumpChooseInterruptTask", "State Jump DefaultNotInterrupt ChooseInterrupt Task"); }
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
	TArray<ECharActionState> BlockInterruptActionState;

	UPROPERTY(EditAnywhere, Category = "StateBlock", meta=(DisplayName="ActionStateBlockInterrupt_Main"))
	TArray<ECharActionState> BlockInterruptActionState_Main;

	UPROPERTY(EditAnywhere, Category = "StateBlock", meta=(DisplayName="ActionStateBlockInterrupt_SP"))
	TArray<ESPActionState> BlockInterruptActionState_SP;

	UPROPERTY(EditAnywhere, Category = "StateBlock", meta=(DisplayName="MotionStateBlockInterrupt(弃用、仅对照)"))
	TArray<ECharMotionState> BlockInterruptMotionState;

	UPROPERTY(EditAnywhere, Category = "StateBlock", meta=(DisplayName="MotionStateBlockInterrupt_Main"))
	TArray<ECharMotionState> BlockInterruptMotionState_Main;

	UPROPERTY(EditAnywhere, Category = "StateBlock", meta=(DisplayName="MotionStateBlockInterrupt_SP"))
	TArray<ESPMotionState> BlockInterruptMotionState_SP;

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
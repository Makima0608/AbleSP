// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UnLuaInterface.h"
#include "CoreMinimal.h"
#include "Game/SPGame/SPGameDefine.h"
#include "MoeGameplay/SceneElements/Others/MoeConveyorCommonBase.h"
#include "Tasks/IAbleAbilityTask.h"
#include "SPStateJumpChooseBlockTask.generated.h"


#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"
/**
 * 
 */
UCLASS()
class FEATURE_SP_API USPStateJumpChooseBlockTask : public UAbleAbilityTask, public IUnLuaInterface
{
	GENERATED_BODY()

protected:
	virtual FString GetModuleName_Implementation() const override;

public:
#if WITH_EDITOR
	virtual FText GetTaskCategory() const override { return LOCTEXT("SPStateJumpChooseBlockTask", "Player"); }

	virtual FText GetTaskName() const override { return LOCTEXT("SPStateJumpChooseBlockTask", "State Jump DefaultOpen ChooseBlock Task"); }
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

	UPROPERTY(EditAnywhere, Category = "StateBlock", meta=(DisplayName="ActionStateBlock(弃用、仅对照)"))
	TArray<ECharActionState> BlockActionState;

	UPROPERTY(EditAnywhere, Category = "StateBlock", meta=(DisplayName="ActionStateBlock_Main"))
	TArray<ECharActionState> BlockActionState_Main;

	UPROPERTY(EditAnywhere, Category = "StateBlock", meta=(DisplayName="ActionStateBlock_SP"))
	TArray<ESPActionState> BlockActionState_SP;

	UPROPERTY(EditAnywhere, Category = "StateBlock", meta=(DisplayName="MotionStateBlock(弃用、仅对照)"))
	TArray<ECharMotionState> BlockMotionState;

	UPROPERTY(EditAnywhere, Category = "StateBlock", meta=(DisplayName="MotionStateBlock_Main"))
	TArray<ECharMotionState> BlockMotionState_Main;

	UPROPERTY(EditAnywhere, Category = "StateBlock", meta=(DisplayName="MotionStateBlock_SP"))
	TArray<ESPMotionState> BlockMotionState_SP;

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
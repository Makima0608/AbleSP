// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UnLuaInterface.h"
#include "CoreMinimal.h"
#include "MoeGameplay/SceneElements/Others/MoeConveyorCommonBase.h"
#include "Tasks/IAbleAbilityTask.h"
#include "Game/SPGame/Character/SPGameCharInputComponent.h"
#include "Game/SPGame/Utils/SPGameLibrary.h"
#include "SPInputControlTask.generated.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"


UENUM(BlueprintType)
enum class EInputControlType : uint8
{
	None = 0 UMETA(DisplayName="无"),
	DisableInput   = 1 UMETA(DisplayName="屏蔽输入"),
	ListenInput	   = 2 UMETA(DisplayName="监听输入打断技能"),
	InputInterruptActionState 	   = 3 UMETA(DisplayName="监听输入打断ActionState"),
};
/**
 * 
 */
UCLASS()
class FEATURE_SP_API USPInputControlTask : public UAbleAbilityTask, public IUnLuaInterface
{
	GENERATED_BODY()

protected:
	virtual FString GetModuleName_Implementation() const override;

public:
#if WITH_EDITOR
	virtual FText GetTaskCategory() const override { return LOCTEXT("SPInputControlTask", "Player"); }

	virtual FText GetTaskName() const { return LOCTEXT("SPInputControlTask", "Input Control Task"); }

	virtual FText GetRichTextTaskSummary() const override;
#endif

	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(USPInputControlTask, STATGROUP_USPAbility);
	}

	virtual bool IsSingleFrame() const override { return IsSingleFrameBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "IsSingleFrame"))
	bool IsSingleFrameBP() const;
	
	virtual bool NeedsTick() const override { return !IsSingleFrame(); }
	
	virtual EAbleAbilityTaskRealm GetTaskRealm() const override { return GetTaskRealmBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "GetTaskRealm"))
	EAbleAbilityTaskRealm GetTaskRealmBP() const;
	
	virtual void OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskStart"))
	void OnTaskStartBP(const UAbleAbilityContext* Context) const;

	virtual void OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context, float deltaTime) const;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskTick"))
	void OnTaskTickBP(const UAbleAbilityContext* Context, float deltaTime) const;

	virtual void OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context, const EAbleAbilityTaskResult result) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskEnd"))
	void OnTaskEndBP(const UAbleAbilityContext* Context, const EAbleAbilityTaskResult result) const;
	
	virtual void OnInput(const TWeakObjectPtr<const UAbleAbilityContext>& Context, ASPGameCharacterBase* InPlayer, float Scale) const;
	
	UPROPERTY(EditAnywhere, Category = "Input Control", meta=(DisplayName="控制类型"))
	EInputControlType InputControlType = EInputControlType::None;

	UPROPERTY(EditAnywhere, Category = "Input Control", meta=(DisplayName="是否使用输入速度"))
	bool bUseControlSpeed = false;

	UPROPERTY(EditAnywhere, Category = "Input Control", meta=(DisplayName="输入打断的ActionState(弃用、仅对照)", EditCondition = "InputControlType == EInputControlType::InputInterruptActionState", EditConditionHides))
	ECharActionState ActionState = ECharActionState::None;

	UPROPERTY(EditAnywhere, Category = "Input Control", meta=(DisplayName="输入打断的ActionState_Main", EditCondition = "InputControlType == EInputControlType::InputInterruptActionState", EditConditionHides))
	ECharActionState ActionState_Main = ECharActionState::None;

	UPROPERTY(EditAnywhere, Category = "Input Control", meta=(DisplayName="输入打断的ActionState_SP", EditCondition = "InputControlType == EInputControlType::InputInterruptActionState", EditConditionHides))
	ESPActionState ActionState_SP = ESPActionState::SP_Begin;

protected:
	void ApplyControlSpeed(const TWeakObjectPtr<const UAbleAbilityContext>& Context, ASPGameCharacterBase* InPlayer, const FVector& InDirection) const;

protected:
	//mutable TArray<USPGameCharInputComponent*> m_InputComps;

	mutable TArray<ASPGameCharacterBase*> m_TargetPlayers;
};


#undef LOCTEXT_NAMESPACE
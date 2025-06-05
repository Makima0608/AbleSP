// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tasks/IAbleAbilityTask.h"
#include "UnLuaInterface.h"
#include "Game/SPGame/Character/SPGameCharInputComponent.h"
#include "Game/SPGame/Utils/SPGameLibrary.h"
#include "SPTwoInputControlTask.generated.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"
/**
 * 
 */
UCLASS()
class FEATURE_SP_API USPTwoInputControlTask : public UAbleAbilityTask, public IUnLuaInterface
{
	GENERATED_BODY()
public:
#if WITH_EDITOR
	virtual FText GetTaskCategory() const override { return LOCTEXT("SPTwoInputControlTask", "Player"); }

	virtual FText GetTaskName() const { return LOCTEXT("SPTwoInputControlTask", "Two Input Control Task"); }
#endif

	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(USPTwoInputControlTask, STATGROUP_USPAbility);
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InputSetting", meta = (ClampMin = 0.0, ClampMax = 1.0, DisplayName = "第一段打断开始，值0~1"))
	float FirstInterruptBegin = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InputSetting", meta = (ClampMin = 0.0, ClampMax = 1.0, DisplayName = "第一段打断结束，值0~1, 值必须大于FirstInterruptBegin"))
	float FirstInterruptEnd = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InputSetting", meta = (ClampMin = 0.0, ClampMax = 1.0, DisplayName = "第二段打断开始，值0~1"))
	float SecondInterruptBegin = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InputSetting", meta = (ClampMin = 0.0, ClampMax = 1.0, DisplayName = "第二段打断结束，值0~1, 值必须大于SecondInterruptBegin"))
	float SecondInterruptEnd = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InputSetting", meta = (DisplayName = "第一段打断优先级, 值越大优先级越高, 如果两个优先级一样，以第一段为主"))
	int FirstPriority = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InputSetting", meta = (DisplayName = "第二段打断优先级, 值越大优先级越高, 如果两个优先级一样，以第一段为主"))
	int SecondPriority = 0;
	
	virtual void OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override;

	virtual void OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context, float deltaTime) const;

	virtual void OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context, const EAbleAbilityTaskResult result) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskStart"))
	void OnTaskStartBP(const UAbleAbilityContext* Context) const;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskTick"))
	void OnTaskTickBP(const UAbleAbilityContext* Context, float deltaTime) const;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskEnd"))
	void OnTaskEndBP(const UAbleAbilityContext* Context, const EAbleAbilityTaskResult result) const;

	virtual void OnInput(const TWeakObjectPtr<const UAbleAbilityContext>& Context, FVector Direction) const;

private:
	virtual FString GetModuleName_Implementation() const override;

	void ApplyControlSpeed(const TWeakObjectPtr<const UAbleAbilityContext>& Context, bool IsUseMaxSpeed) const;

protected:
	mutable USPGameCharInputComponent* InputComp;
};

#undef LOCTEXT_NAMESPACE

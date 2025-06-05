// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UnLuaInterface.h"
#include "CoreMinimal.h"
#include "Tasks/IAbleAbilityTask.h"
#include "Game/SPGame/SubSystem/SPInputManagerSubsystem.h"
#include "SPStateInputCacheTask.generated.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"
/**
 * 
 */
UCLASS()
class FEATURE_SP_API USPStateInputCacheTask : public UAbleAbilityTask, public IUnLuaInterface
{
	GENERATED_BODY()

protected:
	virtual FString GetModuleName_Implementation() const override;

public:
#if WITH_EDITOR
	virtual FText GetTaskCategory() const override { return LOCTEXT("SPStateInputCacheTask", "Player"); }

	virtual FText GetTaskName() const override { return LOCTEXT("SPStateInputCacheTask", "State Input Cache Task"); }
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

	/** 我们想触发的触摸输入类型. The type of input touch we want to trigger */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ESPTouchType TouchType = ESPTouchType::Started;

	/** Deprecated */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayTag InputTag;

	/** 进一步过滤被缓存的输入的标签. 这用于选择触发哪个输入. Tag to further filter inputs that are being cached.  This is used for choosing which input to trigger */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer InputTags;
	
	/** 需要输入相反的操作才能触发动作。只能在按下输入时生效。 The opposite input is required to trigger the action. Only works on pressed input */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool EnableCheckOppositeInput = false;

	/** 即使技能被提前中断，导致输入缓存任务比预期提前结束，仍然允许输入缓存尝试进行模拟。 Allows the input cache to try to simulate, even if the ability is interrupted early, causing the input cache task to end earlier than expected*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool EnableIgnoreAbilityInterrupt = false;
};

#undef LOCTEXT_NAMESPACE
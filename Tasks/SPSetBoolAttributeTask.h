// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UnLuaInterface.h"
#include "CoreMinimal.h"
#include "Tasks/IAbleAbilityTask.h"
#include "SPSetBoolAttributeTask.generated.h"

/**
 * 
 */

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"
/*UCLASS(Transient)
class USPInvincibleTaskScratchPad : public UAbleAbilityTaskScratchPad
{
	GENERATED_BODY()
public:

	bool m_OriginalValue = false;
	
	USPInvincibleTaskScratchPad();
	virtual ~USPInvincibleTaskScratchPad();
	
};*/

UCLASS()
class FEATURE_SP_API USPSetBoolAttributeTask : public UAbleAbilityTask, public IUnLuaInterface
{
	GENERATED_BODY()

protected:
	virtual FString GetModuleName_Implementation() const override;

public:


	
	
	
	USPSetBoolAttributeTask(const FObjectInitializer& ObjectInitializer);
	virtual ~USPSetBoolAttributeTask();

	/* Start our Task. */
	virtual void OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskStart"))
	void OnTaskStartBP(const UAbleAbilityContext* Context) const;
	
	/* End our Task. */
	virtual void OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context, const EAbleAbilityTaskResult result) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskEnd"))
	void OnTaskEndBP(const UAbleAbilityContext* Context, const EAbleAbilityTaskResult result) const;

	/* Returns true if our Task only lasts a single frame. */
	virtual bool IsSingleFrame() const override { return IsSingleFrameBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "IsSingleFrame"))
	bool IsSingleFrameBP() const;

	/* Returns the realm our Task belongs to. */
	virtual EAbleAbilityTaskRealm GetTaskRealm() const override { return GetTaskRealmBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "GetTaskRealm"))
	EAbleAbilityTaskRealm GetTaskRealmBP() const;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Sound", meta = (DisplayName = "Target"))
	TEnumAsByte<EAbleAbilityTargetType> m_Target = EAbleAbilityTargetType::ATT_Self;
	
	UPROPERTY(EditAnywhere, Category = "SetAttribute", meta = (DisplayName = "IsDuration", AbleBindableProperty))
	bool m_IsDuration = false;

	UPROPERTY()
	FGetAbleBool m_IsDurationDelegate;

	UPROPERTY()
	FName m_Attribute;

	UPROPERTY()
	FGetAbleName m_AttributeDelegate;

	UPROPERTY(EditAnywhere, Category = "SetAttribute", meta = (DisplayName = "DurationValue", AbleBindableProperty))
	bool m_SetValue = false;

	UPROPERTY()
	FGetAbleBool m_SetValueDelegate;

	UPROPERTY(EditAnywhere, Category = "SetAttribute", meta = (DisplayName = "是否恢复初始值", AbleBindableProperty, EditCondition = "m_IsDuration==true"))
	bool m_RestoreAtEnd = false;

	UPROPERTY()
	FGetAbleBool m_RestoreAtEndDelegate;

	UPROPERTY(EditAnywhere, Category = "SetAttribute", meta = (DisplayName = "EndValue", AbleBindableProperty, EditCondition = "m_IsDuration == true && m_RestoreAtEnd == false"))
	bool m_ResetValue = false;

	UPROPERTY()
	FGetAbleBool m_ResetValueDelegate;
	

	/* Creates the Scratchpad for this Task. */
	virtual UAbleAbilityTaskScratchPad* CreateScratchPad(const TWeakObjectPtr<UAbleAbilityContext>& Context) const override;

	/* Returns the Profiler Stat ID for this Task. */
	virtual TStatId GetStatId() const override;
	
	virtual void BindDynamicDelegates( UAbleAbility* Ability ) override;

#if WITH_EDITOR
	/* Returns the category of our Task. */
	virtual FText GetTaskCategory() const override { return LOCTEXT("SPSkillSetBoolAttributeTaskCategory", "SetAttribute"); }
	
	/* Returns the name of our Task. */
	virtual FText GetTaskName() const override { return LOCTEXT("SPSkillSetBoolAttributeTask", "SetBoolAttribute"); }

	/* Returns the dynamic, descriptive name of our Task. */
	virtual FText GetDescriptiveTaskName() const override;
	
	/* Returns the description of our Task. */
	virtual FText GetTaskDescription() const override { return LOCTEXT("SPSkillSetBoolAttributeTaskDesc", "SetBoolAttribute."); }

	/* Returns a Rich Text version of the Task summary, for use within the Editor. */
	virtual FText GetRichTextTaskSummary() const;
	
	/* Returns the color of our Task. */
	virtual FLinearColor GetTaskColor() const override { return FLinearColor(0.0f, 0.0f, 220.0f / 255.0f); }
	
	/* Returns the estimated runtime cost of our Task. */
	virtual float GetEstimatedTaskCost() const override { return UAbleAbilityTask::GetEstimatedTaskCost() + 0; } // ABLETASK_EST_SET_ATTRIBUTE; }

	/* Data Validation Tests. */
    EDataValidationResult IsTaskDataValid(const UAbleAbility* AbilityContext, const FText& AssetName, TArray<FText>& ValidationErrors) override;

	/* Returns true if the user is allowed to edit the realm for this Task. */
	virtual bool CanEditTaskRealm() const override { return true; }
#endif
protected:



	/* If true, we'll call the OnSpawnedActorEvent in the Ability Blueprint. */
	UPROPERTY(EditAnywhere, Category = "SetAttribute|Event", meta = (DisplayName = "Fire Event"))
	bool m_FireEvent;

	/* A String identifier you can use to identify this specific task in the Ability blueprint. */
	UPROPERTY(EditAnywhere, Category = "SetAttribute|Event", meta = (DisplayName = "Name", EditCondition = m_FireEvent))
	FName m_Name;

	/* What realm, server or client, to execute this task. If your game isn't networked - this field is ignored. */
	UPROPERTY(EditAnywhere, Category = "Realm", meta = (DisplayName = "Realm",EditCondition = "false"))
	TEnumAsByte<EAbleAbilityTaskRealm> m_TaskRealm;

	
};

#undef LOCTEXT_NAMESPACE



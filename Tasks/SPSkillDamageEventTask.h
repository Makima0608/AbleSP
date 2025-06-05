// Copyright (c) Extra Life Studios, LLC. All rights reserved.

#pragma once

#include "UnLuaInterface.h"
#include "Tasks/IAbleAbilityTask.h"
#include "UObject/ObjectMacros.h"

#include "SPSkillDamageEventTask.generated.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

UCLASS(EditInlineNew, hidecategories=("Targets"))
class FEATURE_SP_API USPSkillDamageEventTask : public UAbleAbilityTask, public IUnLuaInterface
{
	GENERATED_BODY()

protected:
	virtual FString GetModuleName_Implementation() const override;
public:
	USPSkillDamageEventTask(const FObjectInitializer& ObjectInitializer);
	virtual ~USPSkillDamageEventTask();

	/* Start our Task. */
	virtual void OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskStart"))
	void OnTaskStartBP(const UAbleAbilityContext* Context) const;

	/* Returns true if our Task only lasts for a single frame. */
	virtual bool IsSingleFrame() const override { return IsSingleFrameBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "IsSingleFrame"))
	bool IsSingleFrameBP() const;

	/* Returns true if our Task is Async. */
	virtual bool IsAsyncFriendly() const override { return false; }

	/* Setup Dynamic Binding. */
	virtual void BindDynamicDelegates(UAbleAbility* Ability) override;
	
	/* Returns the realm our Task belongs to. */
	virtual EAbleAbilityTaskRealm GetTaskRealm() const override { return GetTaskRealmBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "GetTaskRealm"))
	EAbleAbilityTaskRealm GetTaskRealmBP() const;

	/* Returns the Profiler Stat ID of our Task. */
	virtual TStatId GetStatId() const override;

	virtual void DoGenericEvent(const TWeakObjectPtr<const UAbleAbilityContext>& Context,TArray<TWeakObjectPtr<AActor>>& DamageTargets,TArray<float>& DamageValues,int32 SkillId,int DamageId,AController*& InstigatorController,AActor*& DamageSource) const;

	virtual void SetInstigatorController(const TWeakObjectPtr<const UAbleAbilityContext>& Context,AController*& InstigatorController) const;

	virtual void DoPointDamageEvent(const TWeakObjectPtr<const UAbleAbilityContext>& Context,TArray<TWeakObjectPtr<AActor>>& DamageTargets,AActor*& DamageSource,TArray<float>& DamageValues,AController*& InstigatorController,FVector& ShotDirection,FPointDamageEvent& PointEvent,int32 SkillId,int DamageId) const;
	
	virtual void DoRadialDamageEvent(const TWeakObjectPtr<const UAbleAbilityContext>& Context,TArray<TWeakObjectPtr<AActor>>& DamageTargets,AActor*& DamageSource,TArray<float>& DamageValues,AController*& InstigatorController,FRadialDamageEvent& RadialEvent) const;

	//virtual void AddCalculateTasks(const TWeakObjectPtr<const UAbleAbilityContext>& Context,TArray<TWeakObjectPtr<AActor>>& DamageTargets,TArray<TFuture<float>>& CalculateTasks) const;

	virtual void AddDamageValues(TArray<TFuture<float>>& CalculateTasks,TArray<float>& DamageValues) const;
#if WITH_EDITOR
	/* Returns the category of our Task. */
	virtual FText GetTaskCategory() const override { return LOCTEXT("SPSkillDamageEventCategory", "Damage"); }

	/* Returns the name of our Task. */
	virtual FText GetTaskName() const override { return LOCTEXT("SPSkillDamageEventTask", "Apply Damage"); }

	/* Returns the dynamic, descriptive name of our Task. */
	virtual FText GetDescriptiveTaskName() const override;
	
	/* Returns the description of our Task. */
	virtual FText GetTaskDescription() const override { return LOCTEXT("SPSkillDamageEventTaskDesc", "Calls the OnDamageEvent Blueprint Event on the owning Ability."); }
	
	/* Returns the color of our Task. */
	virtual FLinearColor GetTaskColor() const override { return FLinearColor(153.0f / 255.0f, 38.0f / 255.0f, 20.0f / 255.0f); }
	
	/* Returns estimated runtime cost for our Task. */
	virtual float GetEstimatedTaskCost() const override { return UAbleAbilityTask::GetEstimatedTaskCost() + ABLETASK_EST_BP_EVENT; }

	/* Return how to handle displaying the End time of our Task. */
	virtual EVisibility ShowEndTime() const { return EVisibility::Collapsed; }

	/* Returns true if the user is allowed to edit this Tasks realm.*/
	virtual bool CanEditTaskRealm() const override { return true; }

	/* Data Validation Tests. */
    EDataValidationResult IsTaskDataValid(const UAbleAbility* AbilityContext, const FText& AssetName, TArray<FText>& ValidationErrors) override;
#endif

protected:
	/* Helper method to grab all Actors who will be damaged. */
	void GetDamageTargets(const TWeakObjectPtr<const UAbleAbilityContext>& Context, TArray<TWeakObjectPtr<AActor>>& OutArray) const;

	/* The damage amount. */
	UPROPERTY(EditAnywhere, Category = "Damage", meta = (DisplayName = "Damage Amount"))
	float m_Damage;

	/* Who to set as the "Source" of this damage. */
	UPROPERTY(EditAnywhere, Category = "Damage", meta = (DisplayName = "Damage Source"))
	TEnumAsByte<EAbleAbilityTargetType> m_DamageSource;

	/* Who to apply the damage to. */
	UPROPERTY(EditAnywhere, Category = "Damage", meta = (DisplayName = "Damage Targets"))
	TArray<TEnumAsByte<EAbleAbilityTargetType>> m_DamageTargets;

	/* Damage class, passed along to UE's damage system. Optional. */
	UPROPERTY(EditAnywhere, Category = "Damage", meta= (DisplayName = "Damage Class", AbleBindableProperty))
	TSubclassOf<class UDamageType> m_DamageClass;

	UPROPERTY()
	FGetDamageTypeClass m_DamageClassDelegate;

	UPROPERTY(EditAnywhere, Category = "Damage", meta=(DisplayName = "Radial Damage Info", AbleBindableProperty))
	FRadialDamageParams m_RadialParams;

	UPROPERTY()
	FGetAbleRadialDamageParams m_RadialParamsDelegate;

	UPROPERTY(EditAnywhere, Category = "Damage", meta=(DisplayName = "Damage Index", AbleBindableProperty))
	int32 m_DamageIndex;

	UPROPERTY()
	FGetAbleInt m_DamageIndexDelegate;

	/* A String identifier you can use to identify this specific task in the ability blueprint. */
	UPROPERTY(EditAnywhere, Category = "Damage", meta = (DisplayName = "Event Name"))
	FName m_EventName;

	/* What realm, server or client, to execute this task. If your game isn't networked - this field is ignored. */
	UPROPERTY(EditAnywhere, Category = "Realm", meta = (DisplayName = "Realm"))
	TEnumAsByte<EAbleAbilityTaskRealm> m_TaskRealm;

	/* If true, we will use the Async graph to calculate damage for all Targets across multiple cores. This can speed up execution if the ability
	*  affects a large number of targets and/or the calculations for damage require extensive checks. */
	UPROPERTY(EditAnywhere, Category = "Damage|Optimization", meta = (DisplayName = "Use Async Calculate"))
	bool m_UseAsyncCalculate;
};

#undef LOCTEXT_NAMESPACE
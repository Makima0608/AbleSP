﻿// Copyright (c) Extra Life Studios, LLC. All rights reserved.

#pragma once

#include "UnLuaInterface.h"
#include "Engine/EngineTypes.h"

#include "ableCollisionFilters.h"
#include "ableCollisionSweepTypes.h"
#include "IAbleAbilityTask.h"
#include "UObject/ObjectMacros.h"
#include "WorldCollision.h"

#include "ableCollisionSweepTask.generated.h"

#define LOCTEXT_NAMESPACE "AbleAbilityTask"

/* Scratchpad for our Sweep Task. */
UCLASS(Transient)
class UAbleCollisionSweepTaskScratchPad : public UAbleAbilityTaskScratchPad
{
	GENERATED_BODY()
public:
	UAbleCollisionSweepTaskScratchPad();
	virtual ~UAbleCollisionSweepTaskScratchPad();

	/* The Query Transform. */
	UPROPERTY(Transient)
	FTransform SourceTransform;

	/* Our Async Handle. */
	FTraceHandle AsyncHandle;

	/* Whether or not the Async query has been processed. */
	UPROPERTY(transient)
	bool AsyncProcessed;
};

UCLASS()
class ABLECORESP_API UAbleCollisionSweepTask : public UAbleAbilityTask, public IUnLuaInterface
{
	GENERATED_BODY()

protected:
	virtual FString GetModuleName_Implementation() const override;
public:
	UAbleCollisionSweepTask(const FObjectInitializer& ObjectInitializer);
	virtual ~UAbleCollisionSweepTask();

	/* Start our Task. */
	virtual void OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskStart"))
	void OnTaskStartBP(const UAbleAbilityContext* Context) const;
	
	/* On Task Tick. */
	virtual void OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context, float deltaTime) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskTick"))
	void OnTaskTickBP(const UAbleAbilityContext* Context, float deltaTime) const;
	
	/* End our Task. */
	virtual void OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context, const EAbleAbilityTaskResult result) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskEnd"))
	void OnTaskEndBP(const UAbleAbilityContext* Context, const EAbleAbilityTaskResult result) const;

	/* Returns true if the Task is Async. */
	virtual bool IsAsyncFriendly() const override { return m_SweepShape ? m_SweepShape->IsAsync() && !m_FireEvent : false; }
	
	/* Returns true if the Task only lasts a single frame. */
	virtual bool IsSingleFrame() const override { return IsSingleFrameBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "IsSingleFrame"))
	bool IsSingleFrameBP() const;

	/* Returns true if our Task needs its OnTick method called. */
	virtual bool NeedsTick() const override { return m_SweepShape ? m_SweepShape->IsAsync() : false; }
	
	/* Returns the Realm this Task belongs to. */
	virtual EAbleAbilityTaskRealm GetTaskRealm() const override { return GetTaskRealmBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "GetTaskRealm"))
	EAbleAbilityTaskRealm GetTaskRealmBP() const;

	/* Returns true if our Task is completed. */
	virtual bool IsDone(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "IsDone"))
	bool IsDoneBP(const UAbleAbilityContext* Context) const;

	/* Creates the Scratchpad for our Task. */
	virtual UAbleAbilityTaskScratchPad* CreateScratchPad(const TWeakObjectPtr<UAbleAbilityContext>& Context) const;

	/* Returns the Profiler Stat ID for our Task. */
	virtual TStatId GetStatId() const override;

    /* Returns the Copy Results to Context Parameter. */
    FORCEINLINE bool GetCopyResultsToContext() const { return m_CopyResultsToContext; }

	/* Sweep Shape. */
	const UAbleCollisionSweepShape* GetShape() const { return m_SweepShape; }

	/* Query Filters */
	const TArray<UAbleCollisionFilter*> GetFilters() const { return m_Filters; }
#if WITH_EDITOR
	/* Returns the category for this Task. */
	virtual FText GetTaskCategory() const { return LOCTEXT("AbleCollisionSweepTaskCategory", "Collision"); }
	
	/* Returns the name of this Task. */
	virtual FText GetTaskName() const { return LOCTEXT("AbleCollisionSweepTask", "Collision Sweep"); }

	/* Returns the dynamic, descriptive name of our Task. */
	virtual FText GetDescriptiveTaskName() const override;
	
	/* Returns the description of this Task. */
	virtual FText GetTaskDescription() const { return LOCTEXT("AbleCollisionSweepTaskDesc", "Performs a shape based sweep in the collision scene and returns any entities inside query. The Sweep is executed at the end of the task."); }
	
	/* Returns the color of this Task. */
	virtual FLinearColor GetTaskColor() const { return FLinearColor(56.0f / 255.0f, 118.0f / 255.0f, 29.0f / 255.0f); }
	
	/* Returns the estimated runtime cost of this Task. */
	virtual float GetEstimatedTaskCost() const { return UAbleAbilityTask::GetEstimatedTaskCost() + ABLETASK_EST_COLLISION_SIMPLE_QUERY; }
	
	/* Returns true if the user is allowed to edit the Tasks realm. */
	virtual bool CanEditTaskRealm() const override { return true; }

	/* Returns the Fire Event Parameter. */
	FORCEINLINE bool GetFireEvent() const { return m_FireEvent; }

	/* Data Validation Tests. */
    EDataValidationResult IsTaskDataValid(const UAbleAbility* AbilityContext, const FText& AssetName, TArray<FText>& ValidationErrors) override;

	/* Called by Ability Editor to allow any special logic. */
    void OnAbilityEditorTick(const UAbleAbilityContext& Context, float DeltaTime) const override;

	virtual bool FixUpObjectFlags() override;
#endif

private:
	/* Helper method to copy our query results into the Ability Context. */
	void CopyResultsToContext(const TArray<FAbleQueryResult>& InResults, const TWeakObjectPtr<const UAbleAbilityContext>& Context) const;

	/* Bind our dynamic delegates. */
	virtual void BindDynamicDelegates(UAbleAbility* Ability) override;
protected:
	/* The shape for our sweep. */
	UPROPERTY(EditAnywhere, Instanced, Category = "Sweep|Shape", meta = (DisplayName = "Sweep Shape"))
	UAbleCollisionSweepShape* m_SweepShape = nullptr;

	/* If true, we'll fire the OnCollisionEvent in the Ability Blueprint. */
	UPROPERTY(EditAnywhere, Category = "Sweep|Event", meta = (DisplayName = "Fire Event"))
	bool m_FireEvent;

	/* A String identifier you can use to identify this specific task in the ability blueprint. */
	UPROPERTY(EditAnywhere, Category = "Sweep|Event", meta = (DisplayName = "Name", EditCondition = m_FireEvent))
	FName m_Name;

	/* The Filters to execute on our results. */
	UPROPERTY(EditAnywhere, Instanced, Category = "Sweep|Filter", meta = (DisplayName = "Filters"))
	TArray<UAbleCollisionFilter*> m_Filters;

	/* If true, the results of the query will be added to the Target Actor Array in the Ability Context. Note this takes 1 full frame to complete.*/
	UPROPERTY(EditAnywhere, Category = "Sweep|Misc", meta = (DisplayName = "Copy to Context"))
	bool m_CopyResultsToContext;

	/* If true, we won't check for already existing items when copying results to the context.*/
	UPROPERTY(EditAnywhere, Category = "Sweep|Misc", meta = (DisplayName = "Allow Duplicates", EditCondition = m_CopyResultsToContext))
	bool m_AllowDuplicateEntries;

	/* If true, we'll clear the Target Actor list before copying our context targets in. */
	UPROPERTY(EditAnywhere, Category = "Sweep|Misc", meta = (DisplayName = "Clear Existing Targets", EditCondition = m_CopyResultsToContext))
	bool m_ClearExistingTargets;

	/* What realm, server or client, to execute this task. If your game isn't networked - this field is ignored. */
	UPROPERTY(EditAnywhere, Category = "Realm", meta = (DisplayName = "Realm"))
	TEnumAsByte<EAbleAbilityTaskRealm> m_TaskRealm;
};

#undef LOCTEXT_NAMESPACE
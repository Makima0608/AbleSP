// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UnLuaInterface.h"
#include "CoreMinimal.h"
#include "Tasks/IAbleAbilityTask.h"
#include "SPBranchSegmentTask.generated.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

/**
 * 
 */

UENUM(BlueprintType)
enum class ESPSegmentJumpMode : uint8
{
	Immediately UMETA(DisplayName="立即跳转"),
	AtEnd UMETA(DisplayName="结尾时跳转"),
};

/* Scratchpad for our Task. */
UCLASS(Transient)
class USPBranchSegmentTaskScratchPad : public UAbleAbilityTaskScratchPad
{
	GENERATED_BODY()
public:
	USPBranchSegmentTaskScratchPad();
	virtual ~USPBranchSegmentTaskScratchPad() override;
	
	UPROPERTY(Transient)
	int32 SegmentIndex = -1;
};

UCLASS(EditInlineNew)
class FEATURE_SP_API USPBranchSegmentTask : public UAbleAbilityTask, public IUnLuaInterface
{
	GENERATED_BODY()

protected:
	virtual FString GetModuleName_Implementation() const override;

protected:
	USPBranchSegmentTask(const FObjectInitializer& ObjectInitializer);
	
public:
	/* Called to determine is a Task can start. Default behavior is to simply check if our context time is > our start time. */
	virtual bool CanStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context, float CurrentTime, float DeltaTime) const override;
	
	/* Called as soon as the task is started. Do any per-run initialization here.*/
	virtual void OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskStart"))
	void OnTaskStartBP(const UAbleAbilityContext* Context) const;
	
	/* Called per tick if NeedsTick returns true. Not called on the 1st frame (OnTaskStart is called instead).*/
	virtual void OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context, float deltaTime) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskTick"))
	void OnTaskTickBP(const UAbleAbilityContext* Context, float deltaTime) const;
	
	virtual void OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context, const EAbleAbilityTaskResult result) const override;

	/* Returns true if our Task only lasts for a single frame. */
	virtual bool IsSingleFrame() const override { return Conditions.Num() == 0; }
	
	/* Return true if the task will be ticked each frame during its execution.*/
	virtual bool NeedsTick() const override { return (!IsSingleFrame()); }

	/* Returns the StatId for this Task, used by the Profiler. */
	virtual TStatId GetStatId() const override;

	/* Returns the realm this Task belongs to. */
	virtual EAbleAbilityTaskRealm GetTaskRealm() const override { return GetTaskRealmBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "GetTaskRealm"))
	EAbleAbilityTaskRealm GetTaskRealmBP() const;
	
	virtual void Clear() override;

	virtual void OnBranchSegment(const UAbleAbilityContext* Context, int32 SegmentIndex) const;

	virtual UAbleAbilityTaskScratchPad* CreateScratchPad(const TWeakObjectPtr<UAbleAbilityContext>& Context) const override;

	void CheckAndBranchSegment(const UAbleAbilityContext* Context) const;
	
#if WITH_EDITOR
	virtual bool NeedStartInPreviewTimeSet(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override { return false; }
	
	virtual FText GetTaskCategory() const override { return LOCTEXT("AbleBranchSegmentTask", "Combo"); }
	
	virtual FText GetTaskName() const override { return LOCTEXT("AbleBranchSegmentTask", "BranchSegment"); }
	
	virtual FText GetTaskDescription() const override { return LOCTEXT("AbleBranchSegmentTask", "branch to any segment"); }
	
	virtual FLinearColor GetTaskColor() const override { return FLinearColor(67.0f / 255.0f, 110.0f / 255.0f, 238.0f / 255.0f); }
	
	virtual bool CanEditTaskRealm() const override { return true; }
#endif
	
	UPROPERTY(EditAnywhere, Category = "Segment")
	FName SegmentName;

	UPROPERTY(EditAnywhere, Category = "Segment")
	FName NegateSegmentName;
	
	UPROPERTY(EditAnywhere, Category = "Segment")
	FJumpSegmentSetting JumpSegmentSetting;
	
	UPROPERTY(EditAnywhere, Instanced, Category = "Segment")
	TArray<UAbleChannelingBase*> Conditions;

	UPROPERTY(EditAnywhere, Category = "Segment")
	uint8 bMustPassAllConditions : 1;

	/* What realm, server or client, to execute this task. If your game isn't networked - this field is ignored. */
	UPROPERTY(EditAnywhere, Category = "Realm", meta = (DisplayName = "Realm"))
	TEnumAsByte<EAbleAbilityTaskRealm> m_TaskRealm;
	
	UPROPERTY(EditAnywhere, Category = "Realm", meta = (DisplayName = "CanSimulateRunOnClient"))
	bool bCanSimulateRunOnClient = false;

	UPROPERTY(EditAnywhere, Category = "Segment")
	ESPSegmentJumpMode JumpMode = ESPSegmentJumpMode::Immediately;
};

#undef LOCTEXT_NAMESPACE
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SPWeaponCheckComboTask.h"
#include "UnLuaInterface.h"
#include "Tasks/IAbleAbilityTask.h"
#include "SPInputBranchSegmentTask.generated.h"


#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

/**
 * 
 */

struct FGameplayTag;
class USPGameCharInventoryComponent;

UENUM(BlueprintType)
enum class ESPInputBranchSegmentCheckMode : uint8
{
	Segment,
	Combo,
};

UENUM(BlueprintType)
enum class ESPSegmentInputMode : uint8
{
	Pressed UMETA(ToolTip="按下"),
	Released UMETA(ToolTip="松开"),
	Clicked UMETA(ToolTip="按下后松开"),
	Hold UMETA(ToolTip="长按"),
	UpToDown UMETA(ToolTip="进入时是按下，必须先松开再按下"),
};

UENUM(BlueprintType)
enum class ESPInputBranchSegmentJumpMode : uint8
{
	Immediately UMETA(DisplayName="立即跳转"),
	AtEnd UMETA(DisplayName="结尾时跳转"),
};

UENUM(BlueprintType)
enum class ESPSegmentTaskPhase : uint8
{
	None,
	Checking,
	Passed,
	Failed,
};

/* Scratchpad for our Task. */
UCLASS(Transient)
class USPInputBranchSegmentTaskScratchPad : public UAbleAbilityTaskScratchPad
{
	GENERATED_BODY()
public:
	USPInputBranchSegmentTaskScratchPad();
	virtual ~USPInputBranchSegmentTaskScratchPad() override;
	
	UPROPERTY(Transient)
	int32 SegmentIndex = -1;

	UPROPERTY(Transient)
	float CurrentHoldTime = 0.f;

	UPROPERTY(Transient)
	bool bIsInputPassed = false;

	UPROPERTY(Transient)
	bool bPressedOnStart = false;

	UPROPERTY(Transient)
	bool bPriorityBlocked = false;

	UPROPERTY(Transient)
	TWeakObjectPtr<ASPComboMeleeWeapon> ComboWeapon;

	UPROPERTY(Transient)
	ESPSegmentTaskPhase TaskPhase = ESPSegmentTaskPhase::None;
};

UCLASS(EditInlineNew)
class FEATURE_SP_API USPInputBranchSegmentTask : public UAbleAbilityTask, public IUnLuaInterface
{
	GENERATED_BODY()
	
protected:
	virtual FString GetModuleName_Implementation() const override;

protected:
	USPInputBranchSegmentTask(const FObjectInitializer& ObjectInitializer);

public:
	/* Called to determine is a Task can start. Default behavior is to simply check if our context time is > our start time. */
	virtual bool CanStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context, float CurrentTime, float DeltaTime) const override;

	/* Called as soon as the task is started. Do any per-run initialization here.*/
	virtual void OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskStart"))
	void OnTaskStartBP(UAbleAbilityContext* Context) const;

	/* Called per tick if NeedsTick returns true. Not called on the 1st frame (OnTaskStart is called instead).*/
	virtual void OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context, float deltaTime) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskTick"))
	void OnTaskTickBP(UAbleAbilityContext* Context, float deltaTime) const;
	
	virtual void OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context, const EAbleAbilityTaskResult result) const override;

	/* Return true if the task will be ticked each frame during its execution.*/
	virtual bool NeedsTick() const override { return true; }

	/* Returns the StatId for this Task, used by the Profiler. */
	virtual TStatId GetStatId() const override;

	virtual void Clear() override;

	virtual UAbleAbilityTaskScratchPad* CreateScratchPad(const TWeakObjectPtr<UAbleAbilityContext>& Context) const override;

#if WITH_EDITOR
	virtual bool NeedStartInPreviewTimeSet(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override { return false; }
	
	virtual FText GetTaskCategory() const override { return LOCTEXT("SPInputBranchSegmentTask", "Combo"); }
	
	virtual FText GetTaskName() const override { return LOCTEXT("SPInputBranchSegmentTask", "InputBranchSegment"); }
	
	virtual FText GetTaskDescription() const override { return LOCTEXT("SPInputBranchSegmentTask", "input branch to any segment"); }
	
	virtual FLinearColor GetTaskColor() const override { return FLinearColor(67.0f / 255.0f, 50.0f / 255.0f, 138.0f / 255.0f); }
	
	virtual bool CanEditTaskRealm() const override { return true; }
#endif

	UFUNCTION(BlueprintImplementableEvent)
	bool IsPressed(FGameplayTag InInputTag) const;

protected:
	UFUNCTION(BlueprintNativeEvent)
	bool CheckInput(const UAbleAbilityContext* Context, float DeltaTime) const;

	UFUNCTION(BlueprintNativeEvent)
	bool CheckConditions(const UAbleAbilityContext* Context) const;

	void UpdateResultData(UAbleAbilityContext* Context) const;
	int32 GetResultData(const UAbleAbilityContext* Context) const;
	bool NeedCheckResultData() const;

	virtual void OnBranchSegment(const UAbleAbilityContext* Context, int32 SegmentIndex) const;
	virtual bool OnActivateCombo(const UAbleAbilityContext* Context, ASPComboMeleeWeapon* Weapon) const;
	virtual int32 GetComboIndex(const ASPComboMeleeWeapon* Weapon) const;

protected:
	UPROPERTY(EditAnywhere)
	ESPInputBranchSegmentCheckMode CheckMode = ESPInputBranchSegmentCheckMode::Segment;

	UPROPERTY(EditAnywhere)
	FGameplayTag InputTag = FGameplayTag::EmptyTag;

	UPROPERTY(EditAnywhere)
	ESPSegmentInputMode InputMode = ESPSegmentInputMode::Pressed;

	UPROPERTY(EditAnywhere, meta=(EditCondition = "InputMode == ESPSegmentInputMode::Hold", EditConditionHides))
	float Duration = 0.2f;
	
	UPROPERTY(EditAnywhere)
	ESPInputBranchSegmentJumpMode JumpMode = ESPInputBranchSegmentJumpMode::Immediately;

	UPROPERTY(EditAnywhere, Instanced)
	TArray<UAbleChannelingBase*> Conditions;

	UPROPERTY(EditAnywhere)
	uint8 bMustPassAllConditions : 1;
	
	UPROPERTY(EditAnywhere, Category = "Segment", meta=(EditCondition = "CheckMode == ESPInputBranchSegmentCheckMode::Segment", EditConditionHides))
	FName SegmentName;

	UPROPERTY(EditAnywhere, Category = "Segment", meta=(EditCondition = "CheckMode == ESPInputBranchSegmentCheckMode::Segment", EditConditionHides))
	FJumpSegmentSetting JumpSegmentSetting;

	UPROPERTY(EditAnywhere, Category = "Combo", meta=(DisplayName="连击索引类型", EditCondition = "CheckMode == ESPInputBranchSegmentCheckMode::Combo", EditConditionHides))
	ESPComboIndexMode IndexMode = ESPComboIndexMode::Next;	

	UPROPERTY(EditAnywhere, Category = "Combo", meta=(DisplayName="连击索引", UIMin = 0, ClampMin = 0, EditCondition = "IndexMode == ESPComboIndexMode::Index && CheckMode == ESPInputBranchSegmentCheckMode::Combo", EditConditionHides))
	int32 ComboIndex = 0;

	UPROPERTY(EditAnywhere, Category = "Priority", meta = (DisplayName="Result Name", ToolTip = "存储条件结果项名"))
	FName ResultName = "";

	UPROPERTY(EditAnywhere, Category = "Priority", meta = (DisplayName="Dependency Name", ToolTip = "互斥依赖项名"))
	FName DependencyResultName  = "";
		
	UPROPERTY(EditAnywhere, Category = "Realm", meta = (DisplayName = "CanSimulateRunOnClient"))
	bool bCanSimulateRunOnClient = false;
};

#undef LOCTEXT_NAMESPACE
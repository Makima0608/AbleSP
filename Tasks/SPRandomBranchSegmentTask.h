// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UnLuaInterface.h"
#include "CoreMinimal.h"
#include "Tasks/IAbleAbilityTask.h"
#include "SPRandomBranchSegmentTask.generated.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

USTRUCT(BlueprintType)
struct FRandomBranchSegmentTaskHelper
{
	GENERATED_BODY()

	FRandomBranchSegmentTaskHelper(){}
	FRandomBranchSegmentTaskHelper(FName InSegmentName, int64 InLastChangeTime)
	{
		SegmentName = InSegmentName;
		LastChangeTime = InLastChangeTime;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "RandomBranchSegmentList里的name"))
	FName SegmentName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int64 LastChangeTime = 0;
};

UCLASS(Transient)
class USPRandomBranchSegmentTaskScratchPad : public UAbleAbilityTaskScratchPad
{
	GENERATED_BODY()

public:
	USPRandomBranchSegmentTaskScratchPad();
	virtual ~USPRandomBranchSegmentTaskScratchPad();

	UPROPERTY()
	TArray<FRandomBranchSegmentTaskHelper> SpawnInfos;

    UPROPERTY()
	int32 SegmentIndex = 0;
};

USTRUCT()
struct FRandomBranchSegmentInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "RandomBranchSegmentInfo", meta = (DisplayName = "SegmentName"))
	FName SegmentName;

	UPROPERTY(EditAnywhere, Category = "RandomBranchSegmentInfo", meta = (DisplayName = "CoolDownTime(毫秒)"))
	int64 CoolDownMS = 100;
	
	UPROPERTY(EditAnywhere, Category = "RandomBranchSegmentInfo", meta = (DisplayName = "SegmentWeight(权重)"))
    int32 Weight = 10;
};

/**
 * 随机并跳转的Task
 */
UCLASS(EditInlineNew)
class FEATURE_SP_API USPRandomBranchSegmentTask : public UAbleAbilityTask, public IUnLuaInterface
{
	GENERATED_BODY()

protected:
	virtual FString GetModuleName_Implementation() const override;

protected:
	USPRandomBranchSegmentTask(const FObjectInitializer& ObjectInitializer);
	
public:
	/* Called to determine is a Task can start. Default behavior is to simply check if our context time is > our start time. */
	virtual bool CanStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context, float CurrentTime, float DeltaTime) const;
	
	/* Called as soon as the task is started. Do any per-run initialization here.*/
	virtual void OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskStart"))
	void OnTaskStartBP(const UAbleAbilityContext* Context) const;
	
	virtual void OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context, const EAbleAbilityTaskResult result) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskEnd"))
	void OnTaskEndBP(const UAbleAbilityContext* Context, const EAbleAbilityTaskResult result) const;

	/* Return true if the task will be ticked each frame during its execution.*/
	virtual bool NeedsTick() const { return (!IsSingleFrame() && Conditions.Num() > 0); }

	/* Returns the StatId for this Task, used by the Profiler. */
	virtual TStatId GetStatId() const;

	/* Returns the realm this Task belongs to. */
	virtual EAbleAbilityTaskRealm GetTaskRealm() const override { return GetTaskRealmBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "GetTaskRealm"))
	EAbleAbilityTaskRealm GetTaskRealmBP() const;

	virtual bool IsFillTime() const override { return IsFillTimeBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "IsFillTime"))
	bool IsFillTimeBP() const;

	virtual UAbleAbilityTaskScratchPad* CreateScratchPad(const TWeakObjectPtr<UAbleAbilityContext>& Context) const override;
	
	int32 GenerateRandomIndexByWeight(const UAbleAbilityContext* Context, const TArray<FRandomBranchSegmentInfo>& SegmentList) const;

	bool CanbChangeSegment(const USPRandomBranchSegmentTaskScratchPad* ScratchPad, const FRandomBranchSegmentInfo SegmentInfo) const;

#if WITH_EDITOR
	virtual bool NeedStartInPreviewTimeSet(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override { return false; }
	
	virtual FText GetTaskCategory() const override { return LOCTEXT("AbleChangeSegmentTask", "Segment"); }
	
	virtual FText GetTaskName() const override { return LOCTEXT("AbleChangeSegmentTask", "RandomBranchSegment"); }
	
	virtual FText GetTaskDescription() const override { return LOCTEXT("AbleChangeSegmentTask", "Random Branch to other segment"); }
	
	virtual FLinearColor GetTaskColor() const override { return FLinearColor(67.0f / 255.0f, 110.0f / 255.0f, 238.0f / 255.0f); }
	
	virtual bool CanEditTaskRealm() const override { return true; }
#endif
	
	UPROPERTY(EditAnywhere, Category = "Segment")
	FName SegmentName;

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

protected:
	UPROPERTY(EditAnywhere, Category = "Segment", meta = (DisplayName = "RandomBranchSegmentList"))
	TArray<FRandomBranchSegmentInfo> RandomBranchSegmentList;

};

#undef LOCTEXT_NAMESPACE
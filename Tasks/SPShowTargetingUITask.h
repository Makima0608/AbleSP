// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/SPGame/Skill/Targeting/SPTargetingFilter_ScreenCircle.h"
#include "Tasks/SPAbilityTask.h"
#include "SPShowTargetingUITask.generated.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

/* Scratchpad for our Task. */
UCLASS(Transient, Blueprintable)
class FEATURE_SP_API USPShowTargetingUITaskScratchPad : public USPAbilityTaskScratchPad
{
	GENERATED_BODY()

public:
	USPShowTargetingUITaskScratchPad();

	virtual FString GetModuleName_Implementation() const override;

	const AActor* GetTraceActorByIndex(int32 Index);
	void AddTraceActorByIndex(int32 Index, AActor* TraceActor);
	void RemoveTraceActorByIndex(int32 Index);
	void Reset();
	bool GetActorTraced() const;
	void SetActorTraced(bool bTraced);
	
protected:
	UPROPERTY()
	TMap<int32, TWeakObjectPtr<AActor>> TraceTargetActors;

	UPROPERTY()
	bool bActorTraced = false;
};

USTRUCT(BlueprintType) 
struct FSPShowTargetingUIData
{
	GENERATED_BODY()
public:
	bool IsValidData() const { return !UIName.IsEmpty(); }
	
public:
	UPROPERTY(EditInstanceOnly, Category = "Target")
	FName SocketName = "";

	UPROPERTY(EditInstanceOnly, Category = "Target")
	FString UIName = "";
};

/**
 * 
 */
UCLASS()
class FEATURE_SP_API USPShowTargetingUITask : public USPAbilityTask
{
	GENERATED_BODY()
public:
	virtual void OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override;

	virtual void OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context, float deltaTime) const override;

	virtual void OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context, const EAbleAbilityTaskResult result) const override;
	
	virtual UAbleAbilityTaskScratchPad* CreateScratchPad(const TWeakObjectPtr<UAbleAbilityContext>& Context) const override;
	virtual void ResetScratchPad(UAbleAbilityTaskScratchPad* ScratchPad) const override;
	
	virtual TStatId GetStatId() const override;

	virtual FString GetModuleName_Implementation() const override;

#if WITH_EDITOR
	virtual FText GetTaskName() const override { return LOCTEXT("SPShowTargetingUITask", "ShowTargetingUI"); }
	virtual FText GetTaskCategory() const override { return LOCTEXT("SPShowTargetingUITask", "UI"); }
#endif

	virtual bool IsSingleFrame() const override { return false; }
	
	UFUNCTION(BlueprintImplementableEvent)
	void ShowTargetingUI(float SizeX, float SizeY) const;

	UFUNCTION(BlueprintImplementableEvent)
	void HideTargetingUI() const;

	UFUNCTION(BlueprintImplementableEvent)
	void TraceActor(const AActor* Actor, const FSPShowTargetingUIData& TraceData) const;
	
	UFUNCTION(BlueprintImplementableEvent)
	void UnTraceActor(const AActor* Actor) const;

	UFUNCTION(BlueprintImplementableEvent)
	void SendTraceEvent(bool bTraced) const;
protected:
	void StartTraceTargetActors(const UAbleAbilityContext* Context) const;
	void EndTraceTargetActors(const UAbleAbilityContext* Context) const;
	bool GetTraceDataByIndex(int32 Index, FSPShowTargetingUIData& OutUIData) const;
	
protected:
	UPROPERTY(EditInstanceOnly, Category = "UI")
	FString UIName = "";
	
	UPROPERTY(EditInstanceOnly, Category = "UI", meta = (DisplayName = "形状"))
	ESPFilterScreenType ScreenType = ESPFilterScreenType::Circle;

	UPROPERTY(EditInstanceOnly, Category = "UI", meta = (DisplayName = "值类型"))
	ESPFilterScreenValueType ValueType = ESPFilterScreenValueType::Percentage;
	
	UPROPERTY(EditInstanceOnly, Category = "UI", meta = (DisplayName = "半径", ClampMin=0.1f, EditCondition = "ScreenType == ESPFilterScreenType::Circle", EditConditionHides))
	float Radius = 0.25f;

	UPROPERTY(EditInstanceOnly, Category = "UI", meta = (DisplayName = "宽", ClampMin=0.1f, EditCondition = "ScreenType == ESPFilterScreenType::Rect", EditConditionHides))
	float Width = 0.5f;

	UPROPERTY(EditInstanceOnly, Category = "UI", meta = (DisplayName = "高", ClampMin=0.1f, EditCondition = "ScreenType == ESPFilterScreenType::Rect", EditConditionHides))
	float Height = 0.5f;

	UPROPERTY(EditInstanceOnly, Category = "Target", meta = (DisplayName = "追踪目标数", ClampMin=0))
	int32 TargetCount = 1;

	UPROPERTY(EditInstanceOnly, Category = "Target", meta = (DisplayName = "追踪效果UI"))
	TArray<FSPShowTargetingUIData> TraceUIData;

	UPROPERTY(EditInstanceOnly, Category = "Target", meta = (DisplayName = "是否持续追踪"))
	bool bTickTrace = true;
};

#undef LOCTEXT_NAMESPACE
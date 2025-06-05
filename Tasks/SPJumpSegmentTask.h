// Copyright (c) 2016 - 2019 Extra Life Studios, LLC. All rights reserved.

#pragma once

#include "UnLuaInterface.h"
#include "Tasks/IAbleAbilityTask.h"
#include "UObject/ObjectMacros.h"
#include "SPJumpSegmentTask.generated.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

UCLASS(EditInlineNew)
class FEATURE_SP_API USPJumpSegmentTask : public UAbleAbilityTask, public IUnLuaInterface
{
	GENERATED_BODY()

protected:
	virtual FString GetModuleName_Implementation() const override;
public:
	USPJumpSegmentTask(const FObjectInitializer& ObjectInitializer);

	virtual void OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskStart"))
	void OnTaskStartBP(const UAbleAbilityContext* Context) const;

	virtual bool IsSingleFrame() const override { return IsSingleFrameBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "IsSingleFrame"))
	bool IsSingleFrameBP() const;

	virtual EAbleAbilityTaskRealm GetTaskRealm() const override { return GetTaskRealmBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "GetTaskRealm"))
	EAbleAbilityTaskRealm GetTaskRealmBP() const;

	virtual TStatId GetStatId() const override;

#if WITH_EDITOR
	virtual bool NeedStartInPreviewTimeSet(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override { return false; }
	
	virtual FText GetTaskCategory() const override { return LOCTEXT("AbleJumpSegmentTask", "Combo"); }
	
	virtual FText GetTaskName() const override { return LOCTEXT("AbleJumpSegmentTask", "JumpSegment"); }
	
	virtual FText GetTaskDescription() const override { return LOCTEXT("AbleJumpSegmentTask", "jump to next segment"); }
	
	virtual FLinearColor GetTaskColor() const override { return FLinearColor(67.0f / 255.0f, 110.0f / 255.0f, 238.0f / 255.0f); }
#endif
	
	UPROPERTY(EditAnywhere, Category = "Segment", meta = (DisplayName = "IsSingleFrame"))
	bool m_IsSingleFrame = true;

	UPROPERTY(EditAnywhere, Category = "Segment")
	FJumpSegmentSetting m_JumpSegmentSetting;

	UPROPERTY(EditAnywhere, Category = "Debug", meta = (DisplayName = "TaskRealm"))
	TEnumAsByte<EAbleAbilityTaskRealm> m_TaskRealm = EAbleAbilityTaskRealm::ATR_ClientAndServer;
};

#undef LOCTEXT_NAMESPACE
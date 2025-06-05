// Copyright (c) Extra Life Studios, LLC. All rights reserved.

#pragma once

#include "UnLuaInterface.h"
#include "Game/SPGame/Character/SPAnimStateComponent.h"
#include "MoeGameplay/Animation/AnimModule_MoeBaseMove.h"
#include "Tasks/ablePlayAnimationTask.h"
#include "UObject/ObjectMacros.h"

#include "SPTumblingTask.generated.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

class ASPGameCharacterBase;
enum class EMoeAnimModule_AnimBodyActionState : uint8;
class USPTumblingTaskScratchPad;
class UAnimationAsset;
class UAbleAbilityContext;

FEATURE_SP_API DECLARE_LOG_CATEGORY_EXTERN(LogTumblingTask, Log, All);

UCLASS()
class FEATURE_SP_API USPTumblingTask : public UAblePlayAnimationTask
{
	GENERATED_BODY()

protected:
	virtual FString GetModuleName_Implementation() const override;

public:
	USPTumblingTask(const FObjectInitializer& ObjectInitializer);
	virtual ~USPTumblingTask() override;

	virtual EAbleAbilityTaskRealm GetTaskRealm() const override { return GetTaskRealmBP(); }
	virtual EAbleAbilityTaskRealm GetTaskRealmBP_Implementation() const override;

	virtual void OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override;
	UFUNCTION(BlueprintNativeEvent)
	void OnTaskStartBP_Override(const UAbleAbilityContext* Context) const;

	virtual void OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context, const EAbleAbilityTaskResult result) const override;
	UFUNCTION(BlueprintNativeEvent)
	void OnTaskEndBP_Override(const UAbleAbilityContext* Context, const EAbleAbilityTaskResult result) const;

	virtual void OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context, float deltaTime) const override;
	
#if WITH_EDITOR
	virtual FText GetTaskCategory() const override { return LOCTEXT("SPTumblingTaskCategory", "SPCharacter"); }

	virtual FText GetTaskName() const override { return LOCTEXT("SPTumblingTask", "SPTumbling"); }

	virtual FText GetTaskDescription() const override { return LOCTEXT("SPTumblingDesc", "Plays an Animation asset (currently only Animation Montage and Animation Segments are supported)."); }

	virtual FLinearColor GetTaskColor() const override { return FLinearColor(200.0f / 255.0f, 66.0f / 255.0f, 100.0f / 255.0f); } // Light Blue

#endif

protected:
	UPROPERTY(EditAnywhere, Category = "TumblingStateType", meta=(DisplayName="翻滚类型"))
	EMoeAnimModule_AnimBodyActionState m_TumblingStateType = EMoeAnimModule_AnimBodyActionState::SP_Tumbling;
	
	UFUNCTION(BlueprintNativeEvent)
	bool ChangeCharTumblingSlidingAnim(const ASPGameCharacterBase* SPChar, UAnimationAsset* AnimationAsset) const;

	UFUNCTION(BlueprintNativeEvent)
	void SetPoseState(const ASPGameCharacterBase* SPChar, const bool bStart) const;

	UFUNCTION(BlueprintNativeEvent)
	void ResetPerfectDodge(const ASPGameCharacterBase* SPChar) const;

	UPROPERTY(Transient)
	mutable USPAnimStateComponent* SPAnimStateComp = nullptr;
};

#undef LOCTEXT_NAMESPACE

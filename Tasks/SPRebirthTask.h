// Copyright (c) Extra Life Studios, LLC. All rights reserved.

#pragma once

#include "UnLuaInterface.h"
#include "Game/SPGame/Character/SPAnimStateComponent.h"
#include "Tasks/ablePlayAnimationTask.h"
#include "UObject/ObjectMacros.h"
#include "SPRebirthTask.generated.h"
#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

class ASPGameCharacterBase;
enum class EMoeAnimModule_AnimBodyActionState : uint8;
class USPRebirthTaskScratchPad;
class UAnimationAsset;
class UAbleAbilityContext;


UCLASS()
class FEATURE_SP_API USPRebirthTask : public UAbleAbilityTask, public IUnLuaInterface
{
	GENERATED_BODY()

protected:
	virtual FString GetModuleName_Implementation() const override;

public:
	USPRebirthTask(const FObjectInitializer& ObjectInitializer);
	virtual ~USPRebirthTask() override;

	/* Returns the realm our Task belongs to. */
	virtual EAbleAbilityTaskRealm GetTaskRealm() const override { return GetTaskRealmBP(); }

	virtual TStatId GetStatId() const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "GetTaskRealm"))
	EAbleAbilityTaskRealm GetTaskRealmBP() const;

	virtual void OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override;
	UFUNCTION(BlueprintNativeEvent)
	void OnTaskStartBP_Override(const UAbleAbilityContext* Context) const;

	virtual void OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context, const EAbleAbilityTaskResult result) const override;
	UFUNCTION(BlueprintNativeEvent)
	void OnTaskEndBP_Override(const UAbleAbilityContext* Context, const EAbleAbilityTaskResult result) const;


#if WITH_EDITOR
	virtual FText GetTaskCategory() const override { return LOCTEXT("SPRebirthTaskCategory", "SPRebirth"); }

	virtual FText GetTaskName() const override { return LOCTEXT("SPRebirthTask", "SPRebirth"); }

	virtual FText GetTaskDescription() const override { return LOCTEXT("SPRebirthDesc", "Plays an Animation asset (currently only Animation Montage and Animation Segments are supported)."); }

	virtual FLinearColor GetTaskColor() const override { return FLinearColor(120.0f / 255.0f, 96.0f / 255.0f, 190.0f / 255.0f); } // Light Blue

#endif

protected:
	
};

#undef LOCTEXT_NAMESPACE

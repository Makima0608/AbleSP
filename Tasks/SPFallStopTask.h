// Copyright (c) Extra Life Studios, LLC. All rights reserved.

#pragma once

#include "UnLuaInterface.h"
#include "Game/SPGame/Character/SPAnimStateComponent.h"
#include "Tasks/ablePlayAnimationTask.h"
#include "UObject/ObjectMacros.h"
#include "SPFallStopTask.generated.h"
#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

class USPCharacterMovementComponent;
class USPGameCharInputComponent;
class ASPGameCharacterBase;
enum class EMoeAnimModule_AnimBodyActionState : uint8;
class USPFallStopTaskScratchPad;
class UAnimationAsset;
class UAbleAbilityContext;


UCLASS()
class FEATURE_SP_API USPFallStopTask : public UAbleAbilityTask, public IUnLuaInterface
{
	GENERATED_BODY()

protected:
	virtual FString GetModuleName_Implementation() const override;

public:
	USPFallStopTask(const FObjectInitializer& ObjectInitializer);
	virtual ~USPFallStopTask() override;

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

	virtual void OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context, float deltaTime) const override;
	
	UPROPERTY()
	mutable TWeakObjectPtr<UMoeBuffBase> AddBuff;

	UPROPERTY(EditAnywhere)
	float RotateRate = 1.0f;

#if WITH_EDITOR
	virtual FText GetTaskCategory() const override { return LOCTEXT("SPCharacterTaskCategory", "SPCharacter"); }

	virtual FText GetTaskName() const override { return LOCTEXT("SPFallStopTask", "SPFallStop"); }

	virtual FText GetTaskDescription() const override { return LOCTEXT("SPFallStopDesc", "Change the rotate rate of the  character ."); }

	virtual FLinearColor GetTaskColor() const override { return FLinearColor(160.0f / 255.0f, 136.0f / 255.0f, 210.0f / 255.0f); } // Light Blue

#endif

protected:
	UPROPERTY()
	mutable ASPGameCharacterBase* SPChar = nullptr;

	UPROPERTY()
	mutable USPCharacterMovementComponent* SPMoveComp = nullptr;
	
};

#undef LOCTEXT_NAMESPACE

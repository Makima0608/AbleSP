// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UnLuaInterface.h"
#include "CoreMinimal.h"
#include "MoeGameplay/Character/Buff/MoeBuffBase.h"
#include "Targeting/ableTargetingFilters.h"
#include "Tasks/ableTurnToTask.h"
#include "SPCharacterTurnToTask.generated.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

class UMoeFloatAttributeChangeBuff;

/* Helper Struct */
USTRUCT()
struct FSPTurnToTaskEntry
{
	GENERATED_USTRUCT_BODY()
public:
	FSPTurnToTaskEntry() {};
	FSPTurnToTaskEntry(AActor* InActor, FRotator InTarget)
		: Actor(InActor),
		Target(InTarget)
	{ }
	TWeakObjectPtr<AActor> Actor;
	FRotator Target;
};

/* Scratchpad for our Task. */
UCLASS(Transient)
class USPCharacterTurnToTaskScratchPad : public UAbleAbilityTaskScratchPad
{
	GENERATED_BODY()
public:
	USPCharacterTurnToTaskScratchPad();
	virtual ~USPCharacterTurnToTaskScratchPad();

	/* Any turns in progress. */
	UPROPERTY(transient)
	TArray<FSPTurnToTaskEntry> InProgressTurn;

	/* Blend to use for turns. */
	UPROPERTY(transient)
	FAlphaBlend TurningBlend;
	
	UPROPERTY(transient)
	UMoeFloatAttributeChangeBuff* ChangeYawBuff;

	UPROPERTY(transient)
	float TurnToRotationOffsetTime;

	UPROPERTY(transient)
	float TurnToRotationOffsetBaseTime;
};

UCLASS(EditInlineNew, meta = (DisplayName = "Same Team", ShortToolTip = "Filter Same Team Actors"))
class UAbleAbilityTargetingFilterSameTeam : public UAbleAbilityTargetingFilter
{
	GENERATED_BODY()
public:
	UAbleAbilityTargetingFilterSameTeam(const FObjectInitializer& ObjectInitializer);
	virtual ~UAbleAbilityTargetingFilterSameTeam();

	/* Call into the Ability's CustomFilter method. */
	virtual void Filter(UAbleAbilityContext& Context, const UAbleTargetingBase& TargetBase) const override;
};

UCLASS(EditInlineNew, meta = (DisplayName = "Dead", ShortToolTip = "Filter Dead Actors"))
class UAbleAbilityTargetingFilterDead : public UAbleAbilityTargetingFilter
{
	GENERATED_BODY()
public:
	UAbleAbilityTargetingFilterDead(const FObjectInitializer& ObjectInitializer);
	virtual ~UAbleAbilityTargetingFilterDead();

	/* Call into the Ability's CustomFilter method. */
	virtual void Filter(UAbleAbilityContext& Context, const UAbleTargetingBase& TargetBase) const override;
};

UCLASS()
class FEATURE_SP_API USPCharacterTurnToTask : public UAbleTurnToTask
{
	GENERATED_BODY()

protected:
	virtual FString GetModuleName_Implementation() const override;
public:
	
	USPCharacterTurnToTask(const FObjectInitializer& ObjectInitializer);

	virtual void OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override;
	UFUNCTION(BlueprintNativeEvent)
	void OnTaskStartBP_Override(const UAbleAbilityContext* Context) const;
	
	/* On Task Tick. */
	virtual void OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context, float deltaTime) const override;
	UFUNCTION(BlueprintNativeEvent)
	void OnTaskTickBP_Override(const UAbleAbilityContext* Context, float deltaTime) const;
	
	void CharacterSetMeshYaw(AActor* TargetActor, const FRotator& Rotation, USPCharacterTurnToTaskScratchPad* ScratchPad) const;

	/* End our Task. */
	virtual void OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context, const EAbleAbilityTaskResult result) const override;
	UFUNCTION(BlueprintNativeEvent)
	void OnTaskEndBP_Override(const UAbleAbilityContext* Context, const EAbleAbilityTaskResult result) const;

	virtual TStatId GetStatId() const override;

	virtual UAbleAbilityTaskScratchPad* CreateScratchPad(const TWeakObjectPtr<UAbleAbilityContext>& Context) const override;
#if WITH_EDITOR
	
	virtual FText GetTaskName() const override { return LOCTEXT("USPCharacterTurnToTask", "Character Turn To"); }

	virtual FText GetTaskCategory() const override { return LOCTEXT("USPCharacterTurnToTask", "Character Turn To"); }
	
	virtual bool CanEditTaskRealm() const override { return true; }
#endif

protected:
	virtual FRotator GetTargetRotation(const TWeakObjectPtr<const UAbleAbilityContext>& Context, const AActor* Source, const AActor* Destination) const override;

	FRotator GetTargetRotationByWeaponTrace(const AActor* Source) const;

public:
	UPROPERTY(EditAnywhere, Category = "Turn To", meta = (DisplayName = "UseFastTurn"))
	bool m_bUseFastTurn = false;
	
	UPROPERTY(EditAnywhere, Category = "Turn To", meta = (DisplayName = "LockFaceToFirstFrame"))
	bool m_LockFaceToFirstFrame;

	UPROPERTY(EditAnywhere, Category = "Turn To", meta = (DisplayName = "RotateByWeaponTrace"))
	bool m_RotateByWeaponTrace;

	UPROPERTY(EditAnywhere, Category = "Turn To", meta = (DisplayName = "TurnToTargetRotationOnInterrupt"))
	bool m_TurnToTargetRotationOnInterrupt;

	UPROPERTY(EditAnywhere, Category = "Turn To", meta = (DisplayName = "角色旋转偏移值", EditCondition = "m_RotateByWeaponTrace == true", EditConditionHides))
	FRotator m_TurnToTargetRotationOffset;

	UPROPERTY(EditAnywhere, Category = "Turn To", meta = (DisplayName = "角色转向旋转偏移值的时间", EditCondition = "m_RotateByWeaponTrace == true", EditConditionHides))
	float m_TurnToTargetRotationOffsetTime;
};

#undef LOCTEXT_NAMESPACE
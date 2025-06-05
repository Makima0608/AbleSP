// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tasks/IAbleAbilityTask.h"
#include "SPChangePlayRateTask.generated.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

UENUM(BlueprintType)
enum class ESPChangeRateTarget : uint8
{
	Ability,
};

/* Scratchpad for our Task. */
UCLASS(Transient)
class FEATURE_SP_API USPChangePlayRateTaskScratchPad : public UAbleAbilityTaskScratchPad
{
	GENERATED_BODY()
public:
	USPChangePlayRateTaskScratchPad();
	virtual ~USPChangePlayRateTaskScratchPad();

	float OldPlayRate = -1.0f;
};

/**
 * 
 */
UCLASS()
class FEATURE_SP_API USPChangePlayRateTask : public UAbleAbilityTask
{
	GENERATED_BODY()

public:
	USPChangePlayRateTask(const FObjectInitializer& ObjectInitializer);
	virtual ~USPChangePlayRateTask();

	/* Returns the Profiler Stat ID for this Task. */
	virtual TStatId GetStatId() const override;

	/* Start our Task. */
	virtual void OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override;

	/* End our Task. */
	virtual void OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
						   const EAbleAbilityTaskResult result) const override;

	/* Returns the Realm (Client/Server/Both) that this Task is allowed to execute on. */
	FORCEINLINE virtual EAbleAbilityTaskRealm GetTaskRealm() const override { return m_TaskRealm; }

	/* Creates the Scratchpad for this Task. */
	virtual UAbleAbilityTaskScratchPad* CreateScratchPad(const TWeakObjectPtr<UAbleAbilityContext>& Context) const override;

#if WITH_EDITOR
	virtual FText GetTaskCategory() const override { return LOCTEXT("USPChangePlayRateTask", "Misc"); }
	virtual FText GetTaskName() const override { return LOCTEXT("USPChangePlayRateTask", "Set Ability Play Rate"); }
	virtual bool CanEditTaskRealm() const override { return true; }
#endif

protected:
	virtual void ChangePlayRate(const UAbleAbilityContext* Context) const;
	virtual void RevertPlayRate(const UAbleAbilityContext* Context) const;
	
public:
	UPROPERTY(EditAnywhere)
	ESPChangeRateTarget EffectTarget = ESPChangeRateTarget::Ability;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 0.001))
	float PlayRate = 1.f;

	/* What realm, server or client, to execute this task. If your game isn't networked - this field is ignored. */
	UPROPERTY(EditAnywhere, Category = "Realm", meta = (DisplayName = "Realm"))
	TEnumAsByte<EAbleAbilityTaskRealm> m_TaskRealm = ATR_ClientAndServer;
};

#undef LOCTEXT_NAMESPACE
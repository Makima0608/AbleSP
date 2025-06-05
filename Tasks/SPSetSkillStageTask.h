// Copyright (c) Extra Life Studios, LLC. All rights reserved.

#pragma once

#include "UnLuaInterface.h"
#include "CoreMinimal.h"
#include "Game/SPGame/Monster/AnimInstance/SPMonsterAnimInstance.h"
#include "Game/SPGame/Skill/SPSkillTypes.h"
#include "Tasks/IAbleAbilityTask.h"
#include "SPSetSkillStageTask.generated.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

/**
 * 
 */
UCLASS()
class FEATURE_SP_API USPSetSkillStageTask : public UAbleAbilityTask, public IUnLuaInterface
{
	GENERATED_BODY()

protected:
	virtual FString GetModuleName_Implementation() const override;

public:
	USPSetSkillStageTask(const FObjectInitializer& ObjectInitializer);
	virtual ~USPSetSkillStageTask();

	/* Start our Task. */
	virtual void OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskStart"))
	void OnTaskStartBP(const UAbleAbilityContext* Context) const;
	
	void SetupSkillStartParam(USPMonsterAnimInstance* AnimInstance) const;

	static float GetSequencePlayLength(const USPMonsterAnimInstance* AnimInstance, const FString& PropertyName);

	/* End our Task. */
	virtual void OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	                       const EAbleAbilityTaskResult result) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskEnd"))
	void OnTaskEndBP(const UAbleAbilityContext* Context,
					 const EAbleAbilityTaskResult result) const;

	/* Returns true if our Task only lasts for a single frame. */
	virtual bool IsSingleFrame() const override { return IsSingleFrameBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "IsSingleFrame"))
	bool IsSingleFrameBP() const;

	/* Returns true if our Task needs its tick method called. */
	virtual bool NeedsTick() const override { return false; }

	/* Returns the realm this Task belongs to. */
	virtual EAbleAbilityTaskRealm GetTaskRealm() const override { return GetTaskRealmBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "GetTaskRealm"))
	EAbleAbilityTaskRealm GetTaskRealmBP() const;

	/* Returns the Profiler Stat ID for this Task. */
	virtual TStatId GetStatId() const override;

	/* Setup Dynamic Binding. */
	virtual void BindDynamicDelegates(UAbleAbility* Ability) override;

#if WITH_EDITOR
	/* Returns the category of our Task. */
	virtual FText GetTaskCategory() const override { return LOCTEXT("SPSetSkillStageTask", "Player"); }

	/* Returns the name of our Task. */
	virtual FText GetTaskName() const override { return LOCTEXT("SPSetSkillStageTask", "Set Skill Stage"); }

	/* Returns the description of our Task. */
	virtual FText GetTaskDescription() const override
	{
		return LOCTEXT("SPSetSkillStageTask", "");
	}

	/* Returns the color of our Task. */
	virtual FLinearColor GetTaskColor() const override { return FLinearColor(1.0, 0.69, 0.4f); } // Peach
#endif

	UPROPERTY(EditAnywhere, Category = "AnimMove", meta=(DisplayName="Anim Close Friction", EditCondition = "m_AnimationAsset!=nullptr"))
	bool m_CloseFriction = false;
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Skill Stage",
		meta = (DisplayName = "Skill Stage", AbleBindableProperty))
	ESPSkillStage Stage;

	UPROPERTY()
	FGetAbleInt StageDelegate;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Is Last Stage",
		meta = (DisplayName = "Last Stage", AbleBindableProperty))
	bool Last;

	UPROPERTY(EditAnywhere, Category = "AnimMove", meta=(DisplayName="Enable Anim Move"))
	bool m_EnableAnimMove = true;

	UPROPERTY()
	FGetAbleBool LastDelegate;
};

#undef LOCTEXT_NAMESPACE

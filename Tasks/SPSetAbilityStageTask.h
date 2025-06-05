// Copyright (c) Extra Life Studios, LLC. All rights reserved.

#pragma once

#include "UnLuaInterface.h"
#include "CoreMinimal.h"
#include "Game/SPGame/Monster/AnimInstance/SPMonsterAnimInstance.h"
#include "Game/SPGame/Skill/SPSkillTypes.h"
#include "Tasks/IAbleAbilityTask.h"
#include "SPSetAbilityStageTask.generated.h"

#define LOCTEXT_NAMESPACE "SPAbilityAbilityTask"

UCLASS(Transient)
class USPSetAbilityStageScratchPad : public UAbleAbilityTaskScratchPad
{
	GENERATED_BODY()

public:
	USPSetAbilityStageScratchPad();
	virtual ~USPSetAbilityStageScratchPad();

	UPROPERTY()
	USPMonsterAnimInstance* AnimInstanceCache = nullptr;
	
	UPROPERTY(transient)
	TMap<TWeakObjectPtr<USkeletalMeshComponent>, EVisibilityBasedAnimTickOption> CachedVisibilityBasedAnimTickOptionMap;
};

/**
 * 
 */
UCLASS()
class FEATURE_SP_API USPSeAbilityStageTask : public UAbleAbilityTask, public IUnLuaInterface
{
	GENERATED_BODY()

protected:
	virtual FString GetModuleName_Implementation() const override;

public:
	virtual void ResetScratchPad(UAbleAbilityTaskScratchPad* ScratchPad) const override;
	
	USPSeAbilityStageTask(const FObjectInitializer& ObjectInitializer);
	virtual ~USPSeAbilityStageTask();

	virtual void OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskStart"))
	void OnTaskStartBP(const UAbleAbilityContext* Context) const;

	virtual void OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	                       const EAbleAbilityTaskResult result) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskEnd"))
	void OnTaskEndBP(const UAbleAbilityContext* Context,
					 const EAbleAbilityTaskResult result) const;
	
	virtual bool IsDone(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "IsDone"))
	bool IsDoneBP(const UAbleAbilityContext* Context) const;

	virtual bool IsSingleFrame() const override { return IsSingleFrameBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "IsSingleFrame"))
	bool IsSingleFrameBP() const;

	virtual bool NeedsTick() const override { return false; }

	virtual EAbleAbilityTaskRealm GetTaskRealm() const override { return GetTaskRealmBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "GetTaskRealm"))
	EAbleAbilityTaskRealm GetTaskRealmBP() const;

	/* Returns the Profiler Stat ID for this Task. */
	virtual TStatId GetStatId() const override;

#if WITH_EDITOR
	/* Returns the category of our Task. */
	virtual FText GetTaskCategory() const override { return LOCTEXT("SPSetAbilityStageTask", "Animation"); }

	/* Returns the name of our Task. */
	virtual FText GetTaskName() const override { return LOCTEXT("SPSetAbilityStageTask", "Set Ability Stage"); }

	/* Returns the description of our Task. */
	virtual FText GetTaskDescription() const override
	{
		return LOCTEXT("SetAbilityStageTask", "");
	}

	/* Returns the color of our Task. */
	virtual FLinearColor GetTaskColor() const override { return FLinearColor(1.0, 0.69, 0.4f); } // Peach

	virtual EVisibility ShowEndTime() const override { return m_Adaption ? EVisibility::Hidden : EVisibility::Visible; }
#endif
	
	static FString GetStagePropertyName(const ESPSkillStage Stage);
	
	static float GetSequencePlayLength(const USPMonsterAnimInstance* AnimInstance, const FString& PropertyName);

	static FString GetSequenceName(const USPMonsterAnimInstance* AnimInstance, const FString& PropertyName);

protected:
	virtual UAbleAbilityTaskScratchPad*
	CreateScratchPad(const TWeakObjectPtr<UAbleAbilityContext>& Context) const override;

	UPROPERTY(EditAnywhere, Category = "AnimMove",
		meta=(DisplayName="Anim Close Friction"))
	bool m_CloseFriction = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ability Stage",
		meta = (DisplayName = "Ability Stage"))
	ESPSkillStage Stage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ability Stage",
		meta = (DisplayName = "Auto Adaption"))
	bool m_Adaption = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Is Last Stage",
		meta = (DisplayName = "Last Stage"))
	bool Last;

	UPROPERTY(EditAnywhere, Category = "AnimMove", meta=(DisplayName="Enable Anim Move"))
	bool m_EnableAnimMove = true;

	UPROPERTY(EditAnywhere, Category = "AnimMove", meta=(DisplayName="Z Move By Loc", EditCondition = "false"))
	bool m_OnlyZMoveByLoc = false;

	UPROPERTY(EditAnywhere, Category = "AnimMove", meta=(DisplayName="XY Move By Loc", EditCondition = "false"))
	bool m_OnlyXYMoveByLoc = false;

	UPROPERTY(EditAnywhere, Category = "AnimMove", meta=(DisplayName="Direct Load Xml", EditCondition = "AnimMoveSequence!=nullptr"))
	bool m_DirectLoadXml = true;
	
	UPROPERTY(EditAnywhere, Category = "AnimMove", meta=(DisplayName="Anim Sequence Map"))
	TMap<int32, TSoftObjectPtr<UAnimSequence>> AnimMoveSequenceMap;
	
	/* If true, we'll treat a manually specified length as an interrupt - so normal rules for stopping, clearing the queue, etc apply. */
    UPROPERTY(EditAnywhere, Category = "Animation", meta = (DisplayName = "Override Visibility Based Anim Tick"))
    bool m_OverrideVisibilityBasedAnimTick;
    	
    /* If true, we'll treat a manually specified length as an interrupt - so normal rules for stopping, clearing the queue, etc apply. */
    UPROPERTY(EditAnywhere, Category = "Animation", meta = (DisplayName = "New Visibility Based Anim Tick", EditCondition = "m_OverrideVisibilityBasedAnimTick", EditConditionHides))
    EVisibilityBasedAnimTickOption m_VisibilityBasedAnimTick;
};

#undef LOCTEXT_NAMESPACE

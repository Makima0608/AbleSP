// Copyright (c) Extra Life Studios, LLC. All rights reserved.

#pragma once

#include "UnLuaInterface.h"
#include "CoreMinimal.h"
#include "Tasks/ableCollisionQueryTask.h"
#include "Tasks/IAbleAbilityTask.h"
#include "SPSkillCollisionQueryDamageTask.generated.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

UCLASS(Transient)
class FEATURE_SP_API USPSkillCollisionQueryDamageTaskScratchPad : public UAbleAbilityTaskScratchPad
{
	GENERATED_BODY()

public:
	USPSkillCollisionQueryDamageTaskScratchPad();
	virtual ~USPSkillCollisionQueryDamageTaskScratchPad();

	UPROPERTY(Transient)
	float IntervalTimer = .0f;
	
	UPROPERTY(Transient)
	TSet<AActor*> AddBuffActors;
	
	UPROPERTY(transient)
	FTransform RecordTransform;
};

/**
 * 
 */
UCLASS()
class FEATURE_SP_API USPSkillCollisionQueryDamageTask : public UAbleCollisionQueryTask
{
	GENERATED_BODY()

protected:
	virtual FString GetModuleName_Implementation() const override;
	
public:
	USPSkillCollisionQueryDamageTask(const FObjectInitializer& ObjectInitializer);
	virtual ~USPSkillCollisionQueryDamageTask() override;

	virtual void OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override;
	UFUNCTION(BlueprintNativeEvent)
	void OnTaskStartBP_Override(const UAbleAbilityContext* Context) const;

	virtual void OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context, float deltaTime) const override;
	UFUNCTION(BlueprintNativeEvent)
	void OnTaskTickBP_Override(const UAbleAbilityContext* Context, float deltaTime) const;

	virtual void OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context, const EAbleAbilityTaskResult result) const override;
	UFUNCTION(BlueprintNativeEvent)
	void OnTaskEndBP_Override(const UAbleAbilityContext* Context, const EAbleAbilityTaskResult result) const;

	UFUNCTION(BlueprintNativeEvent)
	void OnCalcMaxDamageCount(const UAbleAbilityContext* Context) const;
	
	virtual bool IsDone(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override;
	virtual bool IsDoneBP_Implementation(const UAbleAbilityContext* Context) const override;

	virtual float GetEndTime() const override
	{
		return UAbleAbilityTask::GetEndTime();
	}
	
	virtual EAbleAbilityTaskRealm GetTaskRealm() const override { return GetTaskRealmBP(); }
	virtual EAbleAbilityTaskRealm GetTaskRealmBP_Implementation() const override;

	virtual UAbleAbilityTaskScratchPad* CreateScratchPad(const TWeakObjectPtr<UAbleAbilityContext>& Context) const override;

	virtual TStatId GetStatId() const override;
	
	virtual void BindDynamicDelegates(UAbleAbility* Ability) override;
	
	virtual bool IsSingleFrame() const override { return IsSingleFrameBP(); }
	virtual bool IsSingleFrameBP_Implementation() const override;

	virtual bool NeedsTick() const override { return !IsSingleFrame(); }
	
	virtual bool IsAsyncFriendly() const override { return false; }
	
#if WITH_EDITOR
	virtual FText GetTaskName() const { return LOCTEXT("SPSkillCollisionQueryDamageTask", "x(已废弃)"); }

	virtual EVisibility ShowEndTime() const { return EVisibility::Visible; }
#endif
	
private:
	void DoQuery(const TWeakObjectPtr<const UAbleAbilityContext>& Context, bool bTickUpdate) const;

	void DoBuffLogic(const TWeakObjectPtr<const UAbleAbilityContext>& Context, AActor* SourceActor, AActor* TargetActor) const;
	
protected:
	UPROPERTY(EditAnywhere, Category = "Damage", meta=(DisplayName = "Damage Index", AbleBindableProperty))
	int32 m_DamageIndex;
	
	UPROPERTY()
	FGetAbleInt m_DamageIndexDelegate;
	
	/* Who to set as the "Source" of this damage. */
	UPROPERTY(EditAnywhere, Category = "Damage", meta = (DisplayName = "Damage Source"))
	TEnumAsByte<EAbleAbilityTargetType> m_DamageSource;
	
	UPROPERTY(EditAnywhere, Category = "Damage", meta=(DisplayName = "Damage Interval", AbleBindableProperty))
	float m_Interval = 1.f;

	UPROPERTY()
	FGetAbleFloat m_IntervalDelegate;

	UPROPERTY(EditAnywhere, Category = "Buff",meta=(DisplayName = "AddBuffWhenDamage"))
	bool m_AddBuffWhenDamage;

	UPROPERTY(EditAnywhere, Category = "Buff" ,meta=(DisplayName = "BuffID",EditCondition = m_AddBuffWhenDamage))
	int m_BuffID;

	UPROPERTY(EditAnywhere, Category = "Buff" ,meta=(DisplayName = "BuffLayer",EditCondition = m_AddBuffWhenDamage))
	int m_BuffLayer;
	
	UPROPERTY(EditAnywhere, Category = "Buff" ,meta=(DisplayName = "AddBuffRepeat",EditCondition = m_AddBuffWhenDamage))
	bool m_AddBuffRepeat;

	UPROPERTY(EditAnywhere, Category = "Buff" ,meta=(DisplayName = "IndexBuffFromSkill",EditCondition = m_AddBuffWhenDamage))
	bool m_IndexFromSkillCfg;

	UPROPERTY(EditAnywhere, Category = "Query|Misc", meta = (DisplayName = "Tick Update Shape"))
	bool m_TickUpdateQueryShape;

	UPROPERTY(EditAnywhere, Category = "Segment", meta = (DisplayName = "HitAndBranchSegment"))
	bool m_bHitAndBranchSegment;

	UPROPERTY(EditAnywhere, Category = "Segment", meta = (DisplayName = "SegmentName", EditCondition = m_bHitAndBranchSegment))
	FName m_SegmentName;
};

#undef LOCTEXT_NAMESPACE
// Copyright (c) Extra Life Studios, LLC. All rights reserved.

#pragma once

#include "UnLuaInterface.h"
#include "CoreMinimal.h"
#include "Game/SPGame/Utils/SPGameLibrary.h"
#include "Tasks/ableCollisionFilters.h"
#include "Tasks/ableCollisionQueryTypes.h"
#include "Tasks/IAbleAbilityTask.h"
#include "SPLaserDamageTask.generated.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

UCLASS(Transient)
class FEATURE_SP_API USPLaserDamageTaskScratchPad : public UAbleAbilityTaskScratchPad
{
	GENERATED_BODY()

public:
	USPLaserDamageTaskScratchPad();

	virtual ~USPLaserDamageTaskScratchPad();

	UPROPERTY(Transient)
	float IntervalTimer = .0f;

	UPROPERTY(transient)
	TWeakObjectPtr<UParticleSystemComponent> SpawnedEffect = nullptr;

	UPROPERTY(transient)
	TArray<FAbleQueryResult> QueryResults;

	UPROPERTY(Transient)
	TSet<AActor*> AddBuffActors;

	UPROPERTY(Transient)
	FTransform OriginTransform;
};

/**
 * 
 */
UCLASS()
class FEATURE_SP_API USPLaserDamageTask : public UAbleAbilityTask, public IUnLuaInterface
{
	GENERATED_BODY()

protected:
	virtual FString GetModuleName_Implementation() const override;

public:
	USPLaserDamageTask(const FObjectInitializer& ObjectInitializer);

	~USPLaserDamageTask();

	virtual void OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskStart"))
	void OnTaskStartBP(const UAbleAbilityContext* Context) const;

	virtual void OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	                        float deltaTime) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskTick"))
	void OnTaskTickBP(const UAbleAbilityContext* Context,
	                        float deltaTime) const;

	virtual void OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	                       const EAbleAbilityTaskResult result) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskEnd"))
	void OnTaskEndBP(const UAbleAbilityContext* Context,
					 const EAbleAbilityTaskResult result) const;

	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(USPLaserDamageTask, STATGROUP_USPAbility);
	}

	virtual bool IsSingleFrame() const override { return IsSingleFrameBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "IsSingleFrame"))
	bool IsSingleFrameBP() const;
	
	virtual bool NeedsTick() const override { return !IsSingleFrame(); }

	virtual EAbleAbilityTaskRealm GetTaskRealm() const override { return GetTaskRealmBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "GetTaskRealm"))
	EAbleAbilityTaskRealm GetTaskRealmBP() const;

	virtual float GetEndTime() const override
	{
		return UAbleAbilityTask::GetEndTime();
	}
	
	virtual EVisibility ShowEndTime() const { return EVisibility::Visible; }

	virtual UAbleAbilityTaskScratchPad*
	CreateScratchPad(const TWeakObjectPtr<UAbleAbilityContext>& Context) const override;
	
	void DoQuery(const TWeakObjectPtr<const UAbleAbilityContext>& Context, bool bTickUpdate) const;
	
	void DoDamage(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const;

	void DoBuffLogic(const TWeakObjectPtr<const UAbleAbilityContext>& Context, AActor* SourceActor, AActor* TargetActor) const;

	void PlayParticleEffect(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const;
	
	void StopParticleEffect(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const;
	
	void RefreshParticleScale(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const;

	float CalculateParticleScale(const TWeakObjectPtr<const UAbleAbilityContext>& Context, const bool SpecialTransform, const FTransform Transform = FTransform()) const;

#if WITH_EDITOR

	virtual FText GetTaskName() const override { return LOCTEXT("USPLaserDamageTask", "Laser(Abandon Stop Use!)"); }

	virtual FText GetTaskCategory() const override { return LOCTEXT("USPLaserDamageTask", "Collision"); }

#endif

protected:
	UPROPERTY(EditAnywhere, Instanced, Category="Query", meta=(DisplayName="Query Shape"))
	UAbleCollisionShape* m_QueryShape;

	UPROPERTY(EditAnywhere, Instanced, Category = "Query|Filter", meta = (DisplayName = "Filters"))
	TArray<UAbleCollisionFilter*> m_Filters;

	UPROPERTY(EditAnywhere, Category="Query", meta=(DisplayName = "Tick Query Shape Change"))
	bool m_TickQueryShapeChange;
	
	UPROPERTY(EditAnywhere, Category = "Damage", meta=(DisplayName = "Damage Index"))
	int32 m_DamageIndex;
	
	UPROPERTY(EditAnywhere, Category = "Damage", meta = (DisplayName = "Damage Source"))
	TEnumAsByte<EAbleAbilityTargetType> m_DamageSource;

	UPROPERTY(EditAnywhere, Category = "Damage", meta = (DisplayName = "Damage Interval"))
	float m_Interval;

	UPROPERTY(EditAnywhere, Category = "Particle", meta = (DisplayName="Effect Template"))
	UParticleSystem* m_EffectTemplate;

	UPROPERTY(EditAnywhere, Category = "Particle", meta=(DisplayName = "Location"))
	FAbleAbilityTargetTypeLocation m_Location;
    
	UPROPERTY(EditAnywhere, Category = "Particle", meta = (DisplayName = "Attach To Socket"))
	bool m_AttachToSocket;

	UPROPERTY(EditAnywhere, Category = "Particle", meta = (DisplayName = "Scale"))
	float m_Scale;

	UPROPERTY(EditAnywhere, Category = "Particle", meta = (DisplayName = "Destroy on End"))
	bool m_DestroyAtEnd;

	UPROPERTY(EditAnywhere, Category = "Particle", meta = (DisplayName = "Normalize Length"))
	float m_ParticleNormalizeLength = 150.f;

	UPROPERTY(EditAnywhere, Category = "Particle", meta = (DisplayName = "Max Length"))
	float m_ParticleMaxLength = 1500.f;

	//Buff
	UPROPERTY(EditAnywhere, Category = "Buff",meta=(DisplayName = "AddBuffWhenDamage"))
	bool m_AddBuffWhenDamage;

	UPROPERTY(EditAnywhere, Category = "Buff" ,meta=(DisplayName = "BuffID", EditCondition = m_AddBuffWhenDamage))
	int m_BuffID;

	UPROPERTY(EditAnywhere, Category = "Buff" ,meta=(DisplayName = "BuffLayer", EditCondition = m_AddBuffWhenDamage))
	int m_BuffLayer;
	
	UPROPERTY(EditAnywhere, Category = "Buff" ,meta=(DisplayName = "AddBuffRepeat", EditCondition = m_AddBuffWhenDamage))
	bool m_AddBuffRepeat;
	
	UPROPERTY(EditAnywhere, Category = "Buff" ,meta=(DisplayName = "IndexBuffFromSkill",EditCondition = m_AddBuffWhenDamage))
	bool m_IndexFromSkillCfg;
};

#undef LOCTEXT_NAMESPACE

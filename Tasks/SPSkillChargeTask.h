// Copyright (c) Extra Life Studios, LLC. All rights reserved.

#pragma once

#include "UnLuaInterface.h"
#include "CoreMinimal.h"
#include "Tasks/IAbleAbilityTask.h"
#include "SPSkillChargeTask.generated.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

UCLASS(Transient)
class USPChargeTaskScratchPad : public UAbleAbilityTaskScratchPad
{
	GENERATED_BODY()

public:
	USPChargeTaskScratchPad();
	
	virtual ~USPChargeTaskScratchPad();

	float ChargedTime = .0f;
};

/**
 * 
 */
UCLASS()
class FEATURE_SP_API USPSkillChargeTask : public UAbleAbilityTask, public IUnLuaInterface
{
	GENERATED_BODY()

protected:
	virtual FString GetModuleName_Implementation() const override;
	
public:
	USPSkillChargeTask(const FObjectInitializer& ObjectInitializer);
	
	virtual ~USPSkillChargeTask();

	virtual void OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskStart"))
	void OnTaskStartBP(const UAbleAbilityContext* Context) const;

	virtual void OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context, float deltaTime) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskTick"))
	void OnTaskTickBP(const UAbleAbilityContext* Context, float deltaTime) const;
	
	virtual void OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
						   const EAbleAbilityTaskResult result) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskEnd"))
	void OnTaskEndBP(const UAbleAbilityContext* Context,
					 const EAbleAbilityTaskResult result) const;

	bool AddImpulse(const AActor* Target, FVector ImpulseForce) const;
	
	virtual bool IsSingleFrame() const override { return IsSingleFrameBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "IsSingleFrame"))
	bool IsSingleFrameBP() const;

	virtual void BindDynamicDelegates(UAbleAbility* Ability) override;

	virtual TStatId GetStatId() const override;

	virtual EAbleAbilityTaskRealm GetTaskRealm() const override { return GetTaskRealmBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "GetTaskRealm"))
	EAbleAbilityTaskRealm GetTaskRealmBP() const;

	virtual UAbleAbilityTaskScratchPad* CreateScratchPad(const TWeakObjectPtr<UAbleAbilityContext>& Context) const override;

#if WITH_EDITOR

	virtual FText GetTaskCategory() const override { return LOCTEXT("USPSkillChargeTask", "Physic"); }

	virtual FText GetTaskName() const override { return LOCTEXT("USPSkillChargeTask", "Charge"); }

	virtual bool CanEditTaskRealm() const override { return true; }
#endif

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Charge", meta = (DisplayName = "Charge Target"))
	TEnumAsByte<EAbleAbilityTargetType> m_Target = EAbleAbilityTargetType::ATT_Self;

	UPROPERTY(EditAnywhere, Category = "Charge", meta = (DisplayName = "Charge Max Impulse"))
	float m_MaxImpulse = 1000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Charge", meta = (DisplayName = "Charge Curve"))
	TSoftObjectPtr<UCurveFloat> ChargeCurve;

	UPROPERTY(EditAnywhere, Category = "Charge", meta = (DisplayName = "Charge Direction"))
	FVector m_Direction = FVector(1.f, 0.f, 0.f);
};

#undef LOCTEXT_NAMESPACE
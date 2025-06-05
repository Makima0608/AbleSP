// Copyright (c) Extra Life Studios, LLC. All rights reserved.

#pragma once

#include "UnLuaInterface.h"
#include "CoreMinimal.h"
#include "Tasks/IAbleAbilityTask.h"
#include "SPSkillAddImpulseTask.generated.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

/**
 * 
 */
UCLASS()
class FEATURE_SP_API USPSkillAddImpulseTask : public UAbleAbilityTask, public IUnLuaInterface
{
	GENERATED_BODY()

protected:
	virtual FString GetModuleName_Implementation() const override;

public:
	USPSkillAddImpulseTask(const FObjectInitializer& ObjectInitializer);
	virtual ~USPSkillAddImpulseTask();

	virtual void OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskStart"))
	void OnTaskStartBP(const UAbleAbilityContext* Context) const;
	
	bool AddImpulseTo(const AActor* Target, FVector ImpulseDirection) const;

	virtual void OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	                       const EAbleAbilityTaskResult result) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskEnd"))
	void OnTaskEndBP(const UAbleAbilityContext* Context,
					 const EAbleAbilityTaskResult result) const;

	virtual bool IsSingleFrame() const override { return IsSingleFrameBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "IsSingleFrame"))
	bool IsSingleFrameBP() const;

	virtual void BindDynamicDelegates(UAbleAbility* Ability) override;

	virtual TStatId GetStatId() const override;

	virtual EAbleAbilityTaskRealm GetTaskRealm() const override { return GetTaskRealmBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "GetTaskRealm"))
	EAbleAbilityTaskRealm GetTaskRealmBP() const;

#if WITH_EDITOR

	virtual FText GetTaskCategory() const override { return LOCTEXT("USPSkillAddImpulseTask", "Physic"); }

	virtual FText GetTaskName() const override { return LOCTEXT("USPSkillAddImpulseTask", "Add Impulse"); }

	virtual bool CanEditTaskRealm() const override { return true; }
#endif

protected:
	/**
	 * 是否受破韧值结果影响
	 */
	UPROPERTY(EditAnywhere, Category = "Impulse", meta = (DisplayName = "Affected By Toughness"))
	bool m_RetStepPoise = true;
	
	UPROPERTY(EditAnywhere, Category = "Impulse", meta = (DisplayName = "Impulse Vector"))
	FVector m_ImpulseVector;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Impulse", meta = (DisplayName = "Impulse Source"))
	TEnumAsByte<EAbleAbilityTargetType> m_Source;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Impulse", meta = (DisplayName = "Impulse Target"))
	TEnumAsByte<EAbleAbilityTargetType> m_Target;
};

#undef LOCTEXT_NAMESPACE

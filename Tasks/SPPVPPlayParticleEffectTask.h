// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tasks/ablePlayParticleEffectTask.h"
#include "Tasks/IAbleAbilityTask.h"
#include "ableAbility.h"
#include "SPPVPPlayParticleEffectTask.generated.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

/**
 * 
 */
UCLASS()
class FEATURE_SP_API USPPVPPlayParticleEffectTask : public UAblePlayParticleEffectTask
{
	GENERATED_BODY()

protected:
	virtual FString GetModuleName_Implementation() const override;

public:
	USPPVPPlayParticleEffectTask(const FObjectInitializer& ObjectInitializer);
	virtual ~USPPVPPlayParticleEffectTask();

	virtual void OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override;
	UFUNCTION(BlueprintNativeEvent)
	void OnTaskStartBP_Override(const UAbleAbilityContext* Context) const;

	virtual void OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context, float deltaTime) const override;
	UFUNCTION(BlueprintNativeEvent)
	void OnTaskTickBP_Override(const UAbleAbilityContext* Context, float deltaTime) const;
	
	virtual void OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context, const EAbleAbilityTaskResult result) const override;
	UFUNCTION(BlueprintNativeEvent)
	void OnTaskEndBP_Override(const UAbleAbilityContext* Context, const EAbleAbilityTaskResult result) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PVP Params", meta = (DisPlayName = "Is Select Capsule Top As Effect Point"))
	bool IsSelectCapsuleTopAsEffectPoint = true;

	UFUNCTION(BlueprintImplementableEvent)
	void SetParticleEffectPos_Lua(const UAbleAbilityContext* Context) const;

#if WITH_EDITOR
	virtual FText GetTaskName() const override { return LOCTEXT("SPPVPPlayParticleEffectTask", "Play PVP Particle Effect"); }
#endif
};

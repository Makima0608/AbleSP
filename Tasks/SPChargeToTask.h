// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tasks/SPAbilityTask.h"
#include "SPChargeToTask.generated.h"

/**
 * 
 */
UCLASS()
class FEATURE_SP_API USPChargeToTask : public USPAbilityTask
{
	GENERATED_BODY()

public:
	virtual void OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const override;
	virtual void OnTaskTickBP_Implementation(const UAbleAbilityContext* Context, float DeltaTime) const override;
	virtual void OnTaskEndBP_Implementation(const UAbleAbilityContext* Context, EAbleAbilityTaskResult result) const override;

protected:
	void OnCharge(const UAbleAbilityContext* Context, ACharacter* Character) const;
	
	template<typename ValueType>
	const ValueType* FindValueByName(const FName& PropertyName) const;
};

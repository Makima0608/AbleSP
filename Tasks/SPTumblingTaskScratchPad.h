// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tasks/IAbleAbilityTask.h"
#include "SPTumblingTaskScratchPad.generated.h"

/**
 * 
 */
UCLASS()
class FEATURE_SP_API USPTumblingTaskScratchPad : public UAbleAbilityTaskScratchPad
{
	GENERATED_BODY()
	
public:
	USPTumblingTaskScratchPad();
	virtual ~USPTumblingTaskScratchPad();

	/* The Ability Components of all the actors we affected. */
	UPROPERTY(transient)
	TArray<TWeakObjectPtr<UAbleAbilityComponent>> AbilityComponents;

	/* The Skeletal Mesh Components of all the actors we affected (Single Node only). */
	UPROPERTY(transient)
	TArray<TWeakObjectPtr<USkeletalMeshComponent>> SingleNodeSkeletalComponents;
	
};

// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SPGame/Skill/Task/SPUIEnergyBarStyleChange.h"

#include "Game/SPGame/Character/SPGameCharacterBase.h"

TStatId USPUIEnergyBarStyleChange::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPUIEnergyBarStyleChange, STATGROUP_USPAbility);
}

FString USPUIEnergyBarStyleChange::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPUIEnergyBarStyleChange");
}

void USPUIEnergyBarStyleChange::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);
	OnTaskStartBP(Context.Get());
}

void USPUIEnergyBarStyleChange::OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const
{
	TArray<TWeakObjectPtr<AActor>> TargetArray;
	GetActorsForTask(Context, TargetArray);
	
	if (TargetArray.Num() <= 0)
	{
		return;
	}

	for(auto Target : TargetArray)
	{
		if (Target.IsValid())
		{
			AActor* TargetActor = Target.Get();

			if (!IsValid(TargetActor))
			{
				return;
			}
			ASPGameCharacterBase* SPChar = Cast<ASPGameCharacterBase>(TargetActor);
			if (!IsValid(SPChar))
				return;

			SPChar->OnSPUIEnergyBarStyleChange.Broadcast(m_StyleType);
		}
	}
}

void USPUIEnergyBarStyleChange::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	const EAbleAbilityTaskResult result) const
{
	Super::OnTaskEnd(Context, result);
	OnTaskEndBP(Context.Get(), result);
}

void USPUIEnergyBarStyleChange::OnTaskEndBP_Implementation(const UAbleAbilityContext* Context,
	                 const EAbleAbilityTaskResult result) const
{
	TArray<TWeakObjectPtr<AActor>> TargetArray;
	GetActorsForTask(Context, TargetArray);
	
	if (TargetArray.Num() <= 0)
	{
		return;
	}

	for(auto Target : TargetArray)
	{
		if (Target.IsValid())
		{
			AActor* TargetActor = Target.Get();

			if (!IsValid(TargetActor))
			{
				return;
			}
			ASPGameCharacterBase* SPChar = Cast<ASPGameCharacterBase>(TargetActor);
			if (!IsValid(SPChar))
				return;

			SPChar->OnSPUIEnergyBarStyleChange.Broadcast(0);
		}
	}
}

EAbleAbilityTaskRealm USPUIEnergyBarStyleChange::GetTaskRealmBP_Implementation() const { return EAbleAbilityTaskRealm::ATR_Client; }

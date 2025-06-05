// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SPGame/Skill/Task/SPUIBlockTask.h"

#include "Game/SPGame/Character/SPGameCharacterBase.h"

TStatId USPUIBlockTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPUIBlockTask, STATGROUP_USPAbility);
}

FString USPUIBlockTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPUIBlockTask");
}

void USPUIBlockTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);
	OnTaskStartBP(Context.Get());
}

void USPUIBlockTask::OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const
{

	ASPGameCharacterBase* SPChar = Cast<ASPGameCharacterBase>(Context->GetSelfActor());
	if (!IsValid(SPChar))
		return;

	SPChar->OnSPUIBlockChange.Broadcast(m_BlockUIType, true);
}

void USPUIBlockTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	const EAbleAbilityTaskResult result) const
{
	Super::OnTaskEnd(Context, result);
	OnTaskEndBP(Context.Get(), result);
}

void USPUIBlockTask::OnTaskEndBP_Implementation(const UAbleAbilityContext* Context,
	                 const EAbleAbilityTaskResult result) const
{

	ASPGameCharacterBase* SPChar = Cast<ASPGameCharacterBase>(Context->GetSelfActor());
	if (!IsValid(SPChar))
		return;

	SPChar->OnSPUIBlockChange.Broadcast(m_BlockUIType, false);
}

EAbleAbilityTaskRealm USPUIBlockTask::GetTaskRealmBP_Implementation() const { return EAbleAbilityTaskRealm::ATR_Client; }

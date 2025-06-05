// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SPGame/Skill/Task/SPRemoveBuffTask.h"

#include "MoeGameLog.h"
#include "Game/SPGame/Gameplay/SPActorInterface.h"
#include "Game/SPGame/Utils/SPGameLibrary.h"

USPRemoveBuffTask::USPRemoveBuffTask(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer),
	m_BuffID(0), m_Layer(1), m_Instigator(ATT_Self)
{
}

FString USPRemoveBuffTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPRemoveBuffTask");
}

void USPRemoveBuffTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);
	OnTaskStartBP(Context.Get());
}

void USPRemoveBuffTask::OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const
{

	if (m_Layer <= 0) return;

	TArray<TWeakObjectPtr<AActor>> Targets;
	GetActorsForTask(Context, Targets);
	if (Targets.Num() <= 0) return;

	const AActor* Instigator = GetSingleActorFromTargetType(Context, m_Instigator);
	for (int i = 0; i < Targets.Num(); ++i)
	{
		if (!Targets[i].IsValid()) continue;

		ISPActorInterface* TargetActor = Cast<ISPActorInterface>(Targets[i].Get());
		if (!TargetActor) continue;

		USPAbilityComponent* AbilityComponent = TargetActor->GetAbilityComponent();
		if (!AbilityComponent) continue;

		AbilityComponent->RemoveBuff(m_BuffID, TargetActor->GetInterfaceOwner(), m_Layer, true);
	}
}

void USPRemoveBuffTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
                                  const EAbleAbilityTaskResult result) const
{
	Super::OnTaskEnd(Context, result);
	OnTaskEndBP(Context.Get(), result);
}

void USPRemoveBuffTask::OnTaskEndBP_Implementation(const UAbleAbilityContext* Context,
                                                   const EAbleAbilityTaskResult result) const
{
}

TStatId USPRemoveBuffTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPRemoveBuffTask, STATGROUP_USPAbility);
}

bool USPRemoveBuffTask::IsSingleFrameBP_Implementation() const { return true; }

EAbleAbilityTaskRealm USPRemoveBuffTask::GetTaskRealmBP_Implementation() const { return ATR_Server; }

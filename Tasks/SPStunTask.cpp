// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SPGame/Skill/Task/SPStunTask.h"
#include "Game/SPGame/Character/SPGameCharacterBase.h"
#include "Game/SPGame/State/StateData/SPStunStateData.h"
#include "Game/SPGame/Utils/SPGameLibrary.h"


#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

USPStunTask::USPStunTask(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	
}

USPStunTask::~USPStunTask()
{
}

FString USPStunTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPStunTask");
}

void USPStunTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);
	OnTaskStartBP(Context.Get());
}

void USPStunTask::OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const
{
	ASPGameCharacterBase* Player = Cast<ASPGameCharacterBase>(Context->GetOwner());
	if (Player && !Player->IsPendingKill())
	{
		auto Data = NewObject<USPStunStateData>();
		if (Data)
		{
			Data->InitializeData(StunTaskTime);
			if (auto StateComp = Player->GetMoeCharStateComponent())
			{
				Player->TryEnterActionState_New(static_cast<uint8>(ESPActionState::SP_Stun), Data, EStateIdOffsetType::SP, true);
			}
		}
	}
}

void USPStunTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	const EAbleAbilityTaskResult result) const
{
	Super::OnTaskEnd(Context, result);
	OnTaskEndBP(Context.Get(), result);
}

void USPStunTask::OnTaskEndBP_Implementation(const UAbleAbilityContext* Context,
	                 const EAbleAbilityTaskResult result) const
{
}

void USPStunTask::BindDynamicDelegates(UAbleAbility* Ability)
{
	Super::BindDynamicDelegates(Ability);
}

TStatId USPStunTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPStunTask, STATGROUP_USPAbility);
}

bool USPStunTask::IsSingleFrameBP_Implementation() const
{
	return true;
}

EAbleAbilityTaskRealm USPStunTask::GetTaskRealmBP_Implementation() const
{
	return EAbleAbilityTaskRealm::ATR_ClientAndServer;
}

#undef LOCTEXT_NAMESPACE
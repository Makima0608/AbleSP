// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SPGame/Skill/Task/SPSetFriendWeaponTask.h"

#include "MoeGameLog.h"
#include "Game/SPGame/Character/SPGameCharacterBase.h"
#include "Game/SPGame/Character/SPGameFriendSkillComponent.h"
#include "Game/SPGame/Character/SPGameMonsterBase.h"
#include "Game/SPGame/Monster/SPGameCharInventoryComponent.h"


USPSetFriendWeaponTask::USPSetFriendWeaponTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	  m_Target(ATT_Self),
	  m_bEquip(true)
{
}

USPSetFriendWeaponTask::~USPSetFriendWeaponTask()
{
}

FString USPSetFriendWeaponTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPSetFriendWeaponTask");
}

void USPSetFriendWeaponTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);
	OnTaskStartBP(Context.Get());
}

void USPSetFriendWeaponTask::OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const
{
	AActor* TaskActor = GetSingleActorFromTargetType(Context, m_Target);
	if (!IsValid(TaskActor))
	{
		return;
	}
	if (ISPActorInterface* SPGameCharacter = Cast<ISPActorInterface>(TaskActor))
	{
		if (USPGameFriendSkillComponent* FriendSkillComponent = SPGameCharacter->GetFriendSkillComponent())
		{
			if (m_bEquip)
			{
				FriendSkillComponent->EquipFriendWeapon_Lua();
			}
			else
			{
				FriendSkillComponent->UnEquipFriendWeapon_Lua();
			}
		}
	}
}

void USPSetFriendWeaponTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context, const EAbleAbilityTaskResult result) const
{
	Super::OnTaskEnd(Context, result);
	OnTaskEndBP(Context.Get(), result);
}

void USPSetFriendWeaponTask::OnTaskEndBP_Implementation(const UAbleAbilityContext* Context, const EAbleAbilityTaskResult result) const
{
}

void USPSetFriendWeaponTask::BindDynamicDelegates(UAbleAbility* Ability)
{
	Super::BindDynamicDelegates(Ability);
}

TStatId USPSetFriendWeaponTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPSetFriendWeaponTask, STATGROUP_USPAbility);
}

bool USPSetFriendWeaponTask::IsSingleFrameBP_Implementation() const { return true; }

EAbleAbilityTaskRealm USPSetFriendWeaponTask::GetTaskRealmBP_Implementation() const { return EAbleAbilityTaskRealm::ATR_ClientAndServer; }

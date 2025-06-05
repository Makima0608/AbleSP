// Copyright (c) Extra Life Studios, LLC. All rights reserved.

#include "Game/SPGame/Skill/Task/SPBroadcastAttackNotifyTask.h"

#include "Game/SPGame/Utils/SPGameLibrary.h"
#include "Game/SPGame/Utils/LuaUtility.h"
#include "Core/LogMacrosEx.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

USPBroadcastAttackNotifyTask::USPBroadcastAttackNotifyTask(const FObjectInitializer& Initializer)
	: Super(Initializer)
	, m_Range(0.0f)
	, m_Angle(180.0f)
	, m_Direction(ESPBroadcastAttackNotifyDirection::Invalid)
{

}

USPBroadcastAttackNotifyTask::~USPBroadcastAttackNotifyTask()
{

}

FString USPBroadcastAttackNotifyTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPBroadcastAttackNotifyTask");
}

void USPBroadcastAttackNotifyTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);
	OnTaskStartBP(Context.Get());
}

void USPBroadcastAttackNotifyTask::OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const
{

	AActor* SelfActor = Context->GetSelfActor();
	if (!IsValid(SelfActor))
	{
		return;
	}
	MOE_LOG(LogTemp, Log, TEXT("[AttackNotify], USPBroadcastAttackNotifyTask::OnTaskStart, Actor %s, Ability %s"), *SelfActor->GetName(), *GetName());

	ASPGameLuaUtility* LuaUtility = USPGameLibrary::GetSPLuaUtility(GetWorld(), SelfActor);
	if (!IsValid(LuaUtility))
	{
		return;
	}
	LuaUtility->BroadcastAttackNotify_Lua(SelfActor, m_Range, m_Angle, m_Direction);
}

void USPBroadcastAttackNotifyTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
											const EAbleAbilityTaskResult result) const
{
	Super::OnTaskEnd(Context, result);
	OnTaskEndBP(Context.Get(), result);
}

void USPBroadcastAttackNotifyTask::OnTaskEndBP_Implementation(const UAbleAbilityContext* Context,
											                 const EAbleAbilityTaskResult result) const
{
}

void USPBroadcastAttackNotifyTask::BindDynamicDelegates(UAbleAbility* Ability)
{
	Super::BindDynamicDelegates(Ability);
}

TStatId USPBroadcastAttackNotifyTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPBroadcastAttackNotifyTask, STATGROUP_USPAbility);
}

bool USPBroadcastAttackNotifyTask::IsSingleFrameBP_Implementation() const { return true; }

EAbleAbilityTaskRealm USPBroadcastAttackNotifyTask::GetTaskRealmBP_Implementation() const	
{
	return EAbleAbilityTaskRealm::ATR_Server;
}

#undef LOCTEXT_NAMESPACE

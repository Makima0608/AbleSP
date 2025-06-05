// Fill out your copyright notice in the Description page of Project Settings.

#include "Game/SPGame/Skill/Task/SPRescueTask.h"
#include "ableSubSystem.h"
#include "Animation/AnimInstanceProxy.h"
#include "Game/SPGame/Character/SPGameCharacterBase.h"
#include "Game/SPGame/Character/SPGameMonsterBase.h"
#include "MoeGameplay/Character/Component/MoeCharAttributeComponent.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

USPRescueTask::USPRescueTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

USPRescueTask::~USPRescueTask()
{
}

TStatId USPRescueTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPRescueTask, STATGROUP_USPAbility);
}

FString USPRescueTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPRescueTask");
}

EAbleAbilityTaskRealm USPRescueTask::GetTaskRealmBP_Implementation() const
{
	return EAbleAbilityTaskRealm::ATR_ClientAndServer;
}

void USPRescueTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
    OnTaskStartBP_Override(Context.Get());
	UAbleAbilityTask::OnTaskStart(Context);
}

void USPRescueTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context, const EAbleAbilityTaskResult result) const
{
    OnTaskEndBP_Override(Context.Get(), result);
	UAbleAbilityTask::OnTaskEnd(Context, result);
}


void USPRescueTask::OnTaskStartBP_Override_Implementation(const UAbleAbilityContext* Context) const
{
	const ASPGameCharacterBase* SPChar = Cast<ASPGameCharacterBase>(Context->GetSelfActor());
	if (!IsValid(SPChar))
	{
		return;
	}
	USPAnimStateComponent* SPAnimStateComp = SPChar->FindComponentByClass<USPAnimStateComponent>();
	if (SPAnimStateComp != nullptr)
	{
		// SPAnimStateComp->AnimLayerPoseState = EMoeAnimModule_AnimPoseState::Moe_Action_Body;

	}

}

void USPRescueTask::OnTaskEndBP_Override_Implementation(const UAbleAbilityContext* Context, const EAbleAbilityTaskResult result) const
{
	const ASPGameCharacterBase* SPChar = Cast<ASPGameCharacterBase>(Context->GetSelfActor());
	if (!IsValid(SPChar))
	{
		return;
	}

}


#undef LOCTEXT_NAMESPACE

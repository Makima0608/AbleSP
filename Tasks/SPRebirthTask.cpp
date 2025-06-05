// Fill out your copyright notice in the Description page of Project Settings.

#include "Game/SPGame/Skill/Task/SPRebirthTask.h"
#include "ableSubSystem.h"
#include "Animation/AnimInstanceProxy.h"
#include "Game/SPGame/Character/SPGameCharacterBase.h"
#include "Game/SPGame/Character/SPGameMonsterBase.h"
#include "MoeGameplay/Character/Component/MoeCharAttributeComponent.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

USPRebirthTask::USPRebirthTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

USPRebirthTask::~USPRebirthTask()
{
}

TStatId USPRebirthTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPRebirthTask, STATGROUP_USPAbility);
}

FString USPRebirthTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPRebirthTask");
}

EAbleAbilityTaskRealm USPRebirthTask::GetTaskRealmBP_Implementation() const
{
	return EAbleAbilityTaskRealm::ATR_ClientAndServer;
}

void USPRebirthTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
    OnTaskStartBP_Override(Context.Get());
	UAbleAbilityTask::OnTaskStart(Context);
}

void USPRebirthTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context, const EAbleAbilityTaskResult result) const
{
    OnTaskEndBP_Override(Context.Get(), result);
	UAbleAbilityTask::OnTaskEnd(Context, result);
}


void USPRebirthTask::OnTaskStartBP_Override_Implementation(const UAbleAbilityContext* Context) const
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

void USPRebirthTask::OnTaskEndBP_Override_Implementation(const UAbleAbilityContext* Context, const EAbleAbilityTaskResult result) const
{
	const ASPGameCharacterBase* SPChar = Cast<ASPGameCharacterBase>(Context->GetSelfActor());
	if (!IsValid(SPChar))
	{
		return;
	}

}


#undef LOCTEXT_NAMESPACE

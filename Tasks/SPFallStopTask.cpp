// Fill out your copyright notice in the Description page of Project Settings.

#include "Game/SPGame/Skill/Task/SPFallStopTask.h"
#include "ableSubSystem.h"
#include "Animation/AnimInstanceProxy.h"
#include "Game/SPGame/Character/SPCharacterMovementComponent.h"
#include "MoeFeatureSPLog.h"
#include "MoeGameLog.h"
#include "Game/SPGame/Character/SPGameCharacterBase.h"
#include "Game/SPGame/Character/SPGameCharInputComponent.h"
#include "Game/SPGame/Character/SPGameMonsterBase.h"
#include "MoeGameplay/Character/Component/MoeCharAttributeComponent.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

USPFallStopTask::USPFallStopTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

USPFallStopTask::~USPFallStopTask()
{
}

TStatId USPFallStopTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPFallStopTask, STATGROUP_USPAbility);
}

FString USPFallStopTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPFallStopTask");
}

EAbleAbilityTaskRealm USPFallStopTask::GetTaskRealmBP_Implementation() const
{
	return EAbleAbilityTaskRealm::ATR_ClientAndServer;
}

void USPFallStopTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
    OnTaskStartBP_Override(Context.Get());
	UAbleAbilityTask::OnTaskStart(Context);
}

void USPFallStopTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context, const EAbleAbilityTaskResult result) const
{
    OnTaskEndBP_Override(Context.Get(), result);
	UAbleAbilityTask::OnTaskEnd(Context, result);
}

void USPFallStopTask::OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context, float deltaTime) const
{
	Super::OnTaskTick(Context, deltaTime);
	if (SPMoveComp != nullptr && SPMoveComp->IsFalling())
	{
		const UAbleAbility* Ability = Context.Get()->GetAbility();
		USPAbilityComponent* AbleAbilityComp = Cast<USPAbilityComponent>(Context.Get()->GetSelfAbilityComponent());
		if (Ability != nullptr && AbleAbilityComp != nullptr)
		{
			const int32 AbilityId = Context.Get()->GetAbilityId();
			AbleAbilityComp->InterruptedAbility(AbilityId, "USPFallStopTask::Falling");
			MOE_SP_ABILITY_LOG(TEXT("USPFallStopTask::Falling InterruptedAbility AbilityId %d"), AbilityId);
		}
	}
}

void USPFallStopTask::OnTaskStartBP_Override_Implementation(const UAbleAbilityContext* Context) const
{ 
	SPChar = Cast<ASPGameCharacterBase>(Context->GetSelfActor());
	if (!IsValid(SPChar))
	{
		return;
	}

	SPMoveComp = SPChar->FindComponentByClass<USPCharacterMovementComponent>();
	
}

void USPFallStopTask::OnTaskEndBP_Override_Implementation(const UAbleAbilityContext* Context, const EAbleAbilityTaskResult result) const
{
	
}


#undef LOCTEXT_NAMESPACE

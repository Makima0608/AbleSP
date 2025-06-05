// Fill out your copyright notice in the Description page of Project Settings.

#include "Game/SPGame/Skill/Task/SPRotateRateTask.h"
#include "ableSubSystem.h"
#include "Animation/AnimInstanceProxy.h"
#include "Game/SPGame/Character/SPCharacterMovementComponent.h"
#include "Game/SPGame/Character/SPGameAttributeComponent.h"
#include "Game/SPGame/Character/SPGameCharacterBase.h"
#include "Game/SPGame/Character/SPGameCharInputComponent.h"
#include "Game/SPGame/Character/SPGameMonsterBase.h"
#include "MoeGameplay/Character/Component/MoeCharAttributeComponent.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

USPRotateRateTask::USPRotateRateTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

USPRotateRateTask::~USPRotateRateTask()
{
}

TStatId USPRotateRateTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPRotateRateTask, STATGROUP_USPAbility);
}

FString USPRotateRateTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPRotateRateTask");
}

EAbleAbilityTaskRealm USPRotateRateTask::GetTaskRealmBP_Implementation() const
{
	return EAbleAbilityTaskRealm::ATR_ClientAndServer;
}

void USPRotateRateTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
    OnTaskStartBP_Override(Context.Get());
	UAbleAbilityTask::OnTaskStart(Context);
}

void USPRotateRateTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context, const EAbleAbilityTaskResult result) const
{
    OnTaskEndBP_Override(Context.Get(), result);
	UAbleAbilityTask::OnTaskEnd(Context, result);
}

void USPRotateRateTask::OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context, float deltaTime) const
{
	Super::OnTaskTick(Context, deltaTime);
	OnTaskTickBP_Override(deltaTime);
}

void USPRotateRateTask::OnTaskTickBP_Override_Implementation(float deltaTime) const
{
	if (SPMoveComp != nullptr && SPChar != nullptr)
	{
		SPMoveComp->UpdateCorrectMoveDirection(FVector::ZeroVector);//修改成同步Mesh朝向的方式
	}
	
	if (SPInputComp != nullptr && SPChar != nullptr)
	{
		if (SPInputComp->GetIsOverrideMove())
		{
			UMoeCharStateComponent* CharStateComponent = SPChar->GetMoeCharStateComponent();
			
			if (CharStateComponent)
			{
				auto StateMachine = CharStateComponent->GetMotionStateMachine();
				if (StateMachine)
				{
					UMoeStateBase* CurState = StateMachine->GetCurrentRuntimeState();
					if (CurState != nullptr)
					{
						UMoeCharMotionStateBase* CurMotionState = Cast<UMoeCharMotionStateBase>(CurState);
						if (CurMotionState != nullptr)
						{
							CurMotionState->SetCharacterMeshRotation(deltaTime);
						}
					}
				}
			}
		}
	}
}


void USPRotateRateTask::OnTaskStartBP_Override_Implementation(const UAbleAbilityContext* Context) const
{ 
	SPChar = Cast<ASPGameCharacterBase>(Context->GetSelfActor());
	if (!IsValid(SPChar))
	{
		return;
	}

	SPMoveComp = SPChar->FindComponentByClass<USPCharacterMovementComponent>();
	SPInputComp = SPChar->FindComponentByClass<USPGameCharInputComponent>();
	USPGameAttributeComponent* SPAttrComp = SPChar->FindComponentByClass<USPGameAttributeComponent>();
	if (SPAttrComp != nullptr)
	{
		UMoeBuffBase* Buff = SPChar->GetCharBuffComponent()->ChangeFloatAttribute(SPChar->GetCharAttributeComponent()->RotationRate, RotateRate);
		AddBuff = Buff;
	}
}

void USPRotateRateTask::OnTaskEndBP_Override_Implementation(const UAbleAbilityContext* Context, const EAbleAbilityTaskResult result) const
{
	if (!IsValid(SPChar))
	{
		return;
	}

	USPGameAttributeComponent* SPAttrComp = SPChar->FindComponentByClass<USPGameAttributeComponent>();
	if (SPAttrComp != nullptr)
	{
		SPChar->GetCharBuffComponent()->RemoveBuff(AddBuff.Get());
	}
}


#undef LOCTEXT_NAMESPACE

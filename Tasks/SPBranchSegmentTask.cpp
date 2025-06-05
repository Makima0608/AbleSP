// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SPGame/Skill/Task/SPBranchSegmentTask.h"
#include "ableAbilityBlueprintLibrary.h"
#include "ableAbilityComponent.h"
#include "ableSubSystem.h"
#include "Game/SPGame/Character/SPGameCharacterBase.h"
#include "Game/SPGame/Utils/SPGameLibrary.h"

USPBranchSegmentTaskScratchPad::USPBranchSegmentTaskScratchPad()
{
}

USPBranchSegmentTaskScratchPad::~USPBranchSegmentTaskScratchPad()
{
}

USPBranchSegmentTask::USPBranchSegmentTask(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer),
	bMustPassAllConditions(true),
	m_TaskRealm(EAbleAbilityTaskRealm::ATR_ClientAndServer)
{
}

bool USPBranchSegmentTask::CanStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context, float CurrentTime,
	float DeltaTime) const
{
	if (!bCanSimulateRunOnClient)
	{
		const ASPGameCharacterBase* Character = Cast<ASPGameCharacterBase>(Context->GetSelfActor());
		if (IsValid(Character))
		{
			const UWorld* World = Character->GetWorld();
			if (IsValid(World) && World->IsClient() && !Character->IsLocallyControlled())
			{
				return false;
			}
		}
	}
	
	return Super::CanStart(Context, CurrentTime, DeltaTime);
}

FString USPBranchSegmentTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPBranchSegmentTask");
}

void USPBranchSegmentTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);
	OnTaskStartBP(Context.Get());
}

void USPBranchSegmentTask::OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const
{
	if (!Context)
	{
		return;
	}
	
	USPBranchSegmentTaskScratchPad* ScratchPad = Cast<USPBranchSegmentTaskScratchPad>(Context->GetScratchPadForTask(this));
	if(ScratchPad == nullptr)
		return;
	
	ScratchPad->SegmentIndex = -1;

	CheckAndBranchSegment(Context);
}

void USPBranchSegmentTask::OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context, float deltaTime) const
{
	Super::OnTaskTick(Context, deltaTime);
	OnTaskTickBP(Context.Get(), deltaTime);
}

void USPBranchSegmentTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	const EAbleAbilityTaskResult result) const
{
	Super::OnTaskEnd(Context, result);

	if (Context.IsValid())
	{
		USPBranchSegmentTaskScratchPad* ScratchPad = Cast<USPBranchSegmentTaskScratchPad>(Context->GetScratchPadForTask(this));
		if(ScratchPad == nullptr)
			return;
	
		if (result == EAbleAbilityTaskResult::Successful && JumpMode == ESPSegmentJumpMode::AtEnd)
		{
			// 正常结束时，勾选结束时跳转
			OnBranchSegment(Context.Get(), ScratchPad->SegmentIndex);
		}

		ScratchPad->SegmentIndex = -1;
	}
}

void USPBranchSegmentTask::OnTaskTickBP_Implementation(const UAbleAbilityContext* Context, float deltaTime) const
{
	CheckAndBranchSegment(Context);
}

void USPBranchSegmentTask::Clear()
{
	Super::Clear();
	for (UAbleChannelingBase* Condition : Conditions)
	{
		if (IsValid(Condition)) Condition->MarkPendingKill();
	}
}

void USPBranchSegmentTask::OnBranchSegment(const UAbleAbilityContext* Context, int32 SegmentIndex) const
{
	if (Context)
	{
		if (SegmentIndex >= 0 && SegmentIndex != Context->GetActiveSegmentIndex())
		{
			if (m_TaskRealm == ATR_Client)
			{
				Context->GetSelfAbilityComponent()->BranchSegmentFromClient(Context, SegmentIndex, JumpSegmentSetting);
			}
			else
			{
				Context->GetSelfAbilityComponent()->BranchSegment(Context, SegmentIndex, JumpSegmentSetting);
			}
		}
	}
}

UAbleAbilityTaskScratchPad* USPBranchSegmentTask::CreateScratchPad(
	const TWeakObjectPtr<UAbleAbilityContext>& Context) const
{
	if (UAbleAbilityUtilitySubsystem* Subsystem = Context->GetUtilitySubsystem())
	{
		static TSubclassOf<UAbleAbilityTaskScratchPad> ScratchPadClass = USPBranchSegmentTaskScratchPad::StaticClass();
		return Subsystem->FindOrConstructTaskScratchPad(ScratchPadClass);
	}
	return NewObject<USPBranchSegmentTaskScratchPad>(Context.Get());
}

void USPBranchSegmentTask::CheckAndBranchSegment(const UAbleAbilityContext* Context) const
{
	if (!IsValid(Context)) return;
	
	int32 SegmentIndex = -1;
	UAbleAbilityContext* ConditionContext = const_cast<UAbleAbilityContext*>(Context);
	if (UAbleAbilityBlueprintLibrary::CheckBranchCond(ConditionContext, Conditions, bMustPassAllConditions))
	{
		if (UAbleAbilityComponent* AbilityComponent = Context->GetSelfAbilityComponent())
		{
			SegmentIndex = AbilityComponent->FindSegment(Context, SegmentName);
		}
	}
	else
	{
		if (UAbleAbilityComponent* AbilityComponent = Context->GetSelfAbilityComponent())
		{
			SegmentIndex = AbilityComponent->FindSegment(Context, NegateSegmentName);
		}
	}

	if (SegmentIndex >= 0 && SegmentIndex != Context->GetActiveSegmentIndex())
	{
		// 分支跳转模式判断：
		// - AtEnd模式缓存索引供结束时使用
		// - Immediately模式立即执行跳转
		if (JumpMode == ESPSegmentJumpMode::AtEnd)
		{
			// 尾部跳转，缓存SegmentIndex
			USPBranchSegmentTaskScratchPad* ScratchPad = Cast<USPBranchSegmentTaskScratchPad>(Context->GetScratchPadForTask(this));
			if(ScratchPad == nullptr)
				return;
			ScratchPad->SegmentIndex = SegmentIndex;
		}
		else
		{
			OnBranchSegment(Context, SegmentIndex);
		}
	}
}

TStatId USPBranchSegmentTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPBranchSegmentTask, STATGROUP_USPAbility);
}

EAbleAbilityTaskRealm USPBranchSegmentTask::GetTaskRealmBP_Implementation() const { return m_TaskRealm; } // Client for Auth client, Server for AIs/Proxies.

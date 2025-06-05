// Copyright (c) 2016 - 2019 Extra Life Studios, LLC. All rights reserved.

#include "Game/SPGame/Skill/Task/SPJumpSegmentTask.h"
#include "ableAbilityComponent.h"
#include "Game/SPGame/Utils/SPGameLibrary.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

USPJumpSegmentTask::USPJumpSegmentTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FString USPJumpSegmentTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPJumpSegmentTask");
}

void USPJumpSegmentTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);
	OnTaskStartBP(Context.Get());
}

void USPJumpSegmentTask::OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const
{

	Context->GetSelfAbilityComponent()->JumpSegment(Context, m_JumpSegmentSetting);
}

TStatId USPJumpSegmentTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPJumpSegmentTask, STATGROUP_USPAbility);
}

bool USPJumpSegmentTask::IsSingleFrameBP_Implementation() const { return m_IsSingleFrame; }

EAbleAbilityTaskRealm USPJumpSegmentTask::GetTaskRealmBP_Implementation() const { return m_TaskRealm; }

#undef LOCTEXT_NAMESPACE

// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SPGame/Skill/Task/SPChangePlayRateTask.h"

#include "ableAbility.h"
#include "ableSubSystem.h"
#include "Game/SPGame/Utils/SPGameLibrary.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

USPChangePlayRateTaskScratchPad::USPChangePlayRateTaskScratchPad()
{
}

USPChangePlayRateTaskScratchPad::~USPChangePlayRateTaskScratchPad()
{
}

USPChangePlayRateTask::USPChangePlayRateTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

USPChangePlayRateTask::~USPChangePlayRateTask()
{
}

TStatId USPChangePlayRateTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPChangePlayRateTask, STATGROUP_USPAbility);
}

void USPChangePlayRateTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);
	ChangePlayRate(Context.Get());
}

void USPChangePlayRateTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	const EAbleAbilityTaskResult result) const
{
	Super::OnTaskEnd(Context, result);
	RevertPlayRate(Context.Get());
}

void USPChangePlayRateTask::RevertPlayRate(const UAbleAbilityContext* Context) const
{
	if (!Context)
		return;
	
	USPChangePlayRateTaskScratchPad* ScratchPad = Cast<USPChangePlayRateTaskScratchPad>(Context->GetScratchPadForTask(this));
	if(ScratchPad == nullptr)
		return;
	
	switch (EffectTarget)
	{
		case ESPChangeRateTarget::Ability:
		{
			if (UAbleAbility* AbilityObj = Context->GetAbilityBP())
			{
				AbilityObj->SetPlayRate(Context, ScratchPad->OldPlayRate);
			}
			break;
		}
	}
}

void USPChangePlayRateTask::ChangePlayRate(const UAbleAbilityContext* Context) const
{
	if (!Context)
		return;
	
	USPChangePlayRateTaskScratchPad* ScratchPad = Cast<USPChangePlayRateTaskScratchPad>(Context->GetScratchPadForTask(this));
	if(ScratchPad == nullptr)
		return;
	
	switch (EffectTarget)
	{
		case ESPChangeRateTarget::Ability:
		{
			if (UAbleAbility* AbilityObj = Context->GetAbilityBP())
			{
				ScratchPad->OldPlayRate = AbilityObj->GetPlayRate(Context);
				AbilityObj->SetPlayRate(Context, PlayRate);
			}
			break;
		}
	}
}

UAbleAbilityTaskScratchPad* USPChangePlayRateTask::CreateScratchPad(
	const TWeakObjectPtr<UAbleAbilityContext>& Context) const
{
	if (UAbleAbilityUtilitySubsystem* Subsystem = Context->GetUtilitySubsystem())
	{
		static TSubclassOf<UAbleAbilityTaskScratchPad> ScratchPadClass =
			USPChangePlayRateTaskScratchPad::StaticClass();
		return Subsystem->FindOrConstructTaskScratchPad(ScratchPadClass);
	}

	return NewObject<USPChangePlayRateTaskScratchPad>(Context.Get());
}

#undef LOCTEXT_NAMESPACE

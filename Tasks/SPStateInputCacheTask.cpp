// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SPGame/Skill/Task/SPStateInputCacheTask.h"

#include "ableAbility.h"
#include "Game/SPGame/SubSystem/SPInputManagerSubsystem.h"
#include "Game/SPGame/Utils/SPGameLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MoeGameLog.h"

TStatId USPStateInputCacheTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPStateInputCacheTask, STATGROUP_USPAbility);
}

FString USPStateInputCacheTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPStateInputCacheTask");
}

void USPStateInputCacheTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);
	OnTaskStartBP(Context.Get());

	if (EnableIgnoreAbilityInterrupt)
	{
		USPInputManagerSubsystem* InputManagerSubsystem = USPInputManagerSubsystem::GetInstance(Context->GetWorld());
		if (InputManagerSubsystem == nullptr)
        	return;
		
		const float CurrentTime = UGameplayStatics::GetTimeSeconds(this);
		const float TaskDeltaTime = m_EndTime - m_StartTime;
		const float TaskEndTime = CurrentTime + TaskDeltaTime;
		const int AbleContextID = Context->GetRandomSeed();
		if (InputTags.IsEmpty())
		{
			if(InputTag.GetTagName() != "None")
			{
				InputManagerSubsystem->SimulateInputWithDelay_Lua(TaskDeltaTime, InputTag, CurrentTime, TaskEndTime, AbleContextID, TouchType, EnableCheckOppositeInput);
			}
		}
		else
		{
			TArray<FGameplayTag> OutInputTags;
			InputTags.GetGameplayTagArray(OutInputTags);
			InputManagerSubsystem->SimulateInputsWithDelay_Lua(TaskDeltaTime, OutInputTags, CurrentTime, TaskEndTime, AbleContextID, TouchType, EnableCheckOppositeInput);
		}
	}
}

void USPStateInputCacheTask::OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const
{
}

void USPStateInputCacheTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	const EAbleAbilityTaskResult result) const
{
    OnTaskEndBP(Context.Get(), result);
	Super::OnTaskEnd(Context, result);
}

void USPStateInputCacheTask::OnTaskEndBP_Implementation(const UAbleAbilityContext* Context,
	                 const EAbleAbilityTaskResult result) const
{
    if (!GetWorld())
		return;

	if (EnableIgnoreAbilityInterrupt)
		return;

	USPInputManagerSubsystem* InputManagerSubsystem = USPInputManagerSubsystem::GetInstance(Context->GetWorld());
	if (InputManagerSubsystem == nullptr)
		return;
	
	const float CurrentGameTime = UGameplayStatics::GetTimeSeconds(this);
	const float TaskStartTime = CurrentGameTime - Context->GetCurrentTime();
	const int AbleContextID = Context->GetRandomSeed();

	if (InputTags.IsEmpty())
	{
		if(InputTag.GetTagName() != "None")
		{
			InputManagerSubsystem->SimulateInputBetweenTimeStamps_Lua(InputTag, TaskStartTime, CurrentGameTime, AbleContextID, TouchType, EnableCheckOppositeInput);
		}
	}
	else
	{
		TArray<FGameplayTag> OutInputTags;
		InputTags.GetGameplayTagArray(OutInputTags);
		InputManagerSubsystem->SimulateInputsWithDelay_Lua(0.0f, OutInputTags, TaskStartTime, CurrentGameTime, AbleContextID, TouchType, EnableCheckOppositeInput);
	}
}

EAbleAbilityTaskRealm USPStateInputCacheTask::GetTaskRealmBP_Implementation() const { return EAbleAbilityTaskRealm::ATR_Client; }

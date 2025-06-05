// Copyright (c) Extra Life Studios, LLC. All rights reserved.

#include "Game/SPGame/Skill/Task/SPConditionCancelAbilityTask.h"

#include "ableAbilityComponent.h"
#include "Game/SPGame/Utils/SPGameLibrary.h"

USPConditionCancelAbilityTask::USPConditionCancelAbilityTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), m_MustPassAllCancelConditions(true), m_PassiveBehavior(), m_CancelResult(Interrupted)
{
}

USPConditionCancelAbilityTask::~USPConditionCancelAbilityTask()
{
}

FString USPConditionCancelAbilityTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPConditionCancelAbilityTask");
}

void USPConditionCancelAbilityTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);
	OnTaskStartBP(Context.Get());
}

void USPConditionCancelAbilityTask::OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const
{
	if (!CheckBranchCondition(Context))
	{
		InternalDoCancel(Context);
	}
}

void USPConditionCancelAbilityTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
                                              const EAbleAbilityTaskResult result) const
{
	Super::OnTaskEnd(Context, result);
	OnTaskEndBP(Context.Get(), result);
}

void USPConditionCancelAbilityTask::OnTaskEndBP_Implementation(const UAbleAbilityContext* Context,
                                                               const EAbleAbilityTaskResult result) const
{
}

TStatId USPConditionCancelAbilityTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPConditionCancelAbilityTask, STATGROUP_USPAbility);
}

/**
 * @deprecated Task discarded
 * @param Context 
 * @return 
 */
bool USPConditionCancelAbilityTask::CheckBranchCondition(
	const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	return true;
	// 此Task废弃
	/*EAbleConditionResults Result = ::ACR_Failed;

	UAbleBranchTaskScratchPad* ScratchPad = nullptr;

	for (UAbleBranchCondition* Condition : m_Conditions)
	{
		if (!Condition)
		{
			continue;
		}

		Result = Condition->CheckCondition(Context, *ScratchPad);
		if (Condition->IsNegated())
		{
			if (Result == ACR_Passed)
			{
				Result = ACR_Failed;
			}
			else if (Result == ACR_Failed)
			{
				Result = ACR_Passed;
			}
		}

		// Check our early out cases.
		if (m_MustPassAllConditions && Result == ACR_Failed)
		{
			// Failed
			break;
		}
		else if (!m_MustPassAllConditions && Result == ACR_Passed)
		{
			// Success
			break;
		}
	}

	return Result == ACR_Passed;*/
}

void USPConditionCancelAbilityTask::InternalDoCancel(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	TArray<TWeakObjectPtr<AActor>> TaskTargets;
	GetActorsForTask(Context, TaskTargets);

	for (TWeakObjectPtr<AActor> TargetActor : TaskTargets)
	{
		if (!TargetActor.IsValid())
		{
			continue;
		}

		if (UAbleAbilityComponent* AbilityComponent = TargetActor->FindComponentByClass<UAbleAbilityComponent>())
		{
			if (const UAbleAbility* ActiveAbility = AbilityComponent->GetActiveAbility())
			{
				if (ShouldCancelAbility(*ActiveAbility, *Context.Get()))
				{
					AbilityComponent->CancelAbility(Context->GetAbility(), m_CancelResult.GetValue());
				}
			}

			TArray<UAbleAbility*> CurrentPassives;
			AbilityComponent->GetCurrentPassiveAbilities(CurrentPassives);

			for (UAbleAbility* Passive : CurrentPassives)
			{
				if (ShouldCancelAbility(*Passive, *Context.Get()))
				{
					switch (m_PassiveBehavior.GetValue())
					{
					case RemoveOneStack:
					case RemoveOneStackWithRefresh:
						{
							int32 StackCount = AbilityComponent->GetCurrentStackCountForPassiveAbility(Passive);
							int32 NewStackCount = FMath::Max(StackCount - 1, 0);
							AbilityComponent->SetPassiveStackCount(Passive, NewStackCount,
							                                       m_PassiveBehavior.GetValue() ==
							                                       RemoveOneStackWithRefresh,
							                                       m_CancelResult.GetValue());
						}
						break;
					case RemoveEntireStack:
					default:
						{
							AbilityComponent->CancelAbility(Passive, m_CancelResult.GetValue());
						}
						break;
					}
				}
			}
		}
	}
}

bool USPConditionCancelAbilityTask::ShouldCancelAbility(const UAbleAbility& Ability,
                                                        const UAbleAbilityContext& Context) const
{
	if (Context.GetAbility() && Context.GetAbility()->GetAbilityNameHash() == Ability.GetAbilityNameHash())
	{
		return true;
	}

	return false;
}

bool USPConditionCancelAbilityTask::IsSingleFrameBP_Implementation() const { return true; }

EAbleAbilityTaskRealm USPConditionCancelAbilityTask::GetTaskRealmBP_Implementation() const { return ATR_ClientAndServer; }

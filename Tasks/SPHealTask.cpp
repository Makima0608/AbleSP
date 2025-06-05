// Copyright (c) Extra Life Studios, LLC. All rights reserved.

#include "Game/SPGame/Skill/Task/SPHealTask.h"
#include "Game/SPGame/Utils/SPGameLibrary.h"

USPHealTask::USPHealTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), m_HealValue(0), m_HealSource(ATT_Self)
{
}

USPHealTask::~USPHealTask()
{
}

FString USPHealTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPHealTask");
}

void USPHealTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);
	OnTaskStartBP(Context.Get());
}

void USPHealTask::OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const
{

	AActor* HealSource = GetSingleActorFromTargetType(Context, m_HealSource);
	TArray<TWeakObjectPtr<AActor>> HealTargets;
	GetHealTargets(Context, HealTargets);

	if (HealTargets.Num())
	{
		TArray<float> DamageValues;
		DamageValues.Reserve(HealTargets.Num());

		int32 SkillId = Context->GetAbilityId();

		AController* InstigatorController = nullptr;
		if (AActor* InstigatorActor = Context->GetInstigator())
		{
			if (InstigatorActor->IsA<AController>())
			{
				InstigatorController = Cast<AController>(InstigatorActor);
			}
			else if (InstigatorActor->IsA<APawn>())
			{
				InstigatorController = Cast<APawn>(InstigatorActor)->GetController();
			}
		}

		for (int32 i = 0; i < HealTargets.Num(); ++i)
		{
			if (HealTargets[i].IsValid())
			{
				USPGameLibrary::DoHeal(SkillId, m_HealValue, HealTargets[i].Get(), InstigatorController, HealSource);
			}
		}
	}
}

void USPHealTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
                            const EAbleAbilityTaskResult result) const
{
	Super::OnTaskEnd(Context, result);
	OnTaskEndBP(Context.Get(), result);
}

void USPHealTask::OnTaskEndBP_Implementation(const UAbleAbilityContext* Context,
                                             const EAbleAbilityTaskResult result) const
{
}

TStatId USPHealTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPHealTask, STATGROUP_USPAbility);
}

void USPHealTask::GetHealTargets(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
                                 TArray<TWeakObjectPtr<AActor>>& OutArray) const
{
	for (const EAbleAbilityTargetType Target : m_HealTargets)
	{
		switch (Target)
		{
		case EAbleAbilityTargetType::ATT_Self:
		case EAbleAbilityTargetType::ATT_Owner:
		case EAbleAbilityTargetType::ATT_Instigator:
			{
				if (AActor* Actor = GetSingleActorFromTargetType(Context, Target))
				{
					OutArray.Add(Actor);
				}
			}
			break;
		case EAbleAbilityTargetType::ATT_TargetActor:
			{
				const TArray<TWeakObjectPtr<AActor>>& UnfilteredTargets = Context->GetTargetActorsWeakPtr();
				for (const TWeakObjectPtr<AActor>& TempTarget : UnfilteredTargets)
				{
					if (IsTaskValidForActor(TempTarget.Get()))
					{
						OutArray.Add(TempTarget);
					}
				}
			}
			break;
		default:
			break;
		}
	}
}

bool USPHealTask::IsSingleFrameBP_Implementation() const { return true; }

EAbleAbilityTaskRealm USPHealTask::GetTaskRealmBP_Implementation() const { return ATR_Server; }

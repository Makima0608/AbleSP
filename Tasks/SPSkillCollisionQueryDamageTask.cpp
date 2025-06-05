// Copyright (c) Extra Life Studios, LLC. All rights reserved.

#include "Game/SPGame/Skill/Task/SPSkillCollisionQueryDamageTask.h"
#include "ableAbility.h"
#include "ableAbilityComponent.h"
#include "ableSubSystem.h"
#include "MoeGameLog.h"
#include "Game/SPGame/Gameplay/SPActorInterface.h"
#include "Game/SPGame/Skill/Core/SPAbilityFunctionLibrary.h"
#include "Game/SPGame/Character/SPGameCharacterBase.h"
#include "Game/SPGame/Utils/SPGameLibrary.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"


USPSkillCollisionQueryDamageTaskScratchPad::USPSkillCollisionQueryDamageTaskScratchPad(): IntervalTimer(0)
{
}

USPSkillCollisionQueryDamageTaskScratchPad::~USPSkillCollisionQueryDamageTaskScratchPad()
{
}

USPSkillCollisionQueryDamageTask::USPSkillCollisionQueryDamageTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), m_DamageIndex(0), m_DamageSource(), m_AddBuffWhenDamage(false), m_BuffID(0),
	  m_BuffLayer(0), m_AddBuffRepeat(false),
	  m_IndexFromSkillCfg(false), m_TickUpdateQueryShape(true),m_bHitAndBranchSegment(false),m_SegmentName(NAME_None)
{
}

USPSkillCollisionQueryDamageTask::~USPSkillCollisionQueryDamageTask()
{
}

FString USPSkillCollisionQueryDamageTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPSkillCollisionQueryDamageTask");
}

void USPSkillCollisionQueryDamageTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	const UAbleAbilityContext* ContextPtr = Context.Get();
	OnTaskStartBP_Override(ContextPtr);
	OnCalcMaxDamageCount(ContextPtr);
}

void USPSkillCollisionQueryDamageTask::OnTaskStartBP_Override_Implementation(const UAbleAbilityContext* Context) const
{
    USPSkillCollisionQueryDamageTaskScratchPad* ScratchPad = Cast<USPSkillCollisionQueryDamageTaskScratchPad>(
		Context->GetScratchPadForTask(this));
	if (!ScratchPad) return;
	const float IntervalValue = m_Interval;
	ScratchPad->IntervalTimer = IntervalValue;

	DoQuery(Context, false);
}

void USPSkillCollisionQueryDamageTask::OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
                                                  float deltaTime) const
{
	OnTaskTickBP_Override(Context.Get(), deltaTime);
}

void USPSkillCollisionQueryDamageTask::OnTaskTickBP_Override_Implementation(const UAbleAbilityContext* Context,
	float deltaTime) const
{
    const float DeltaTimeWithRate = deltaTime;
	USPSkillCollisionQueryDamageTaskScratchPad* ScratchPad = Cast<USPSkillCollisionQueryDamageTaskScratchPad>(
		Context->GetScratchPadForTask(this));
	if (!ScratchPad) return;
	
	ScratchPad->IntervalTimer -= DeltaTimeWithRate;
	if (ScratchPad->IntervalTimer <= 0.f)
	{
		const float IntervalValue = m_Interval;
		ScratchPad->IntervalTimer = IntervalValue;
		DoQuery(Context, true);
	}
}

void USPSkillCollisionQueryDamageTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
                                                 const EAbleAbilityTaskResult result) const
{
	OnTaskEndBP_Override(Context.Get(), result);
}

void USPSkillCollisionQueryDamageTask::OnTaskEndBP_Override_Implementation(const UAbleAbilityContext* Context,
	const EAbleAbilityTaskResult result) const
{
    USPSkillCollisionQueryDamageTaskScratchPad* ScratchPad = Cast<USPSkillCollisionQueryDamageTaskScratchPad>(
		Context->GetScratchPadForTask(this));
	if (!ScratchPad) return;

	ScratchPad->IntervalTimer = 0.f;
	ScratchPad->AddBuffActors.Empty();
}

void USPSkillCollisionQueryDamageTask::OnCalcMaxDamageCount_Implementation(const UAbleAbilityContext* Context) const
{
	
}

bool USPSkillCollisionQueryDamageTask::IsDone(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	return IsDoneBP(Context.Get());
}

bool USPSkillCollisionQueryDamageTask::IsDoneBP_Implementation(const UAbleAbilityContext* Context) const
{
	return UAbleAbilityTask::IsDone(Context);
}

UAbleAbilityTaskScratchPad* USPSkillCollisionQueryDamageTask::CreateScratchPad(
	const TWeakObjectPtr<UAbleAbilityContext>& Context) const
{
	if (UAbleAbilityUtilitySubsystem* Subsystem = Context->GetUtilitySubsystem())
	{
		static TSubclassOf<UAbleAbilityTaskScratchPad> ScratchPadClass =
			USPSkillCollisionQueryDamageTaskScratchPad::StaticClass();
		return Subsystem->FindOrConstructTaskScratchPad(ScratchPadClass);
	}

	return NewObject<USPSkillCollisionQueryDamageTaskScratchPad>(Context.Get());
}

TStatId USPSkillCollisionQueryDamageTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPSkillCollisionQueryDamageTask, STATGROUP_USPAbility);
}

void USPSkillCollisionQueryDamageTask::BindDynamicDelegates(UAbleAbility* Ability)
{
	Super::BindDynamicDelegates(Ability);
	ABL_BIND_DYNAMIC_PROPERTY(Ability, m_DamageIndex, TEXT("Damage Index"));
	ABL_BIND_DYNAMIC_PROPERTY(Ability, m_Interval, TEXT("Damage Interval"));
}

void USPSkillCollisionQueryDamageTask::DoQuery(const TWeakObjectPtr<const UAbleAbilityContext>& Context, bool bTickUpdate) const
{
	if (m_QueryShape)
	{
		TArray<FAbleQueryResult> Results;
		if (m_TickUpdateQueryShape)
		{
			m_QueryShape->DoQuery(Context, Results);
		}
		else
		{
			USPSkillCollisionQueryDamageTaskScratchPad* ScratchPad = Cast<USPSkillCollisionQueryDamageTaskScratchPad>(Context->GetScratchPadForTask(this));
			if (!ScratchPad) return;
			if (!bTickUpdate)
			{
				FTransform StartTransform = m_QueryShape->DoQuery(Context, Results);
				ScratchPad->RecordTransform = StartTransform;
			}
			else
			{
				m_QueryShape->DoQuery(Context, Results, ScratchPad->RecordTransform, true);
			}
		}
		UE_LOG(LogTemp, Log, TEXT("Collision Query Task Result, %i"), Results.Num())
		if (Results.Num()) //|| (m_CopyResultsToContext && m_ClearExistingTargets))
		{
			for (const UAbleCollisionFilter* CollisionFilter : m_Filters)
			{
				// 防止配置上多出了一些None的Filter
				if (!IsValid(CollisionFilter)) continue;
				CollisionFilter->Filter(Context, Results);
			}

			if (Results.Num())
			{
				const int32 AbilityId = Context->GetAbilityId();
				const int32 AbilityUniqueID = Context->GetAbilityUniqueID();
				int32 DamageIndex = ABL_GET_DYNAMIC_PROPERTY_VALUE(Context, m_DamageIndex);
				FSPSkillInfo SkillInfo;
				USPAbilityFunctionLibrary::GetAbilityInfo(Context->GetWorld(), AbilityId, SkillInfo);
				
				TArray<int32> DamageIds = SkillInfo.DamageIds;
				int32 DamageId = DamageIds.IsValidIndex(DamageIndex) ? DamageIds[DamageIndex] : INDEX_NONE;
				AController* InstigatorController = nullptr;
				if (AActor* InstigatorActor = Context->GetInstigator())
				{
					if (InstigatorActor->IsA<AController>())
						InstigatorController = Cast<AController>(InstigatorActor);
					else if (InstigatorActor->IsA<APawn>())
						InstigatorController = Cast<APawn>(InstigatorActor)->GetController();
				}
				AActor* DamageSource = GetSingleActorFromTargetType(Context, m_DamageSource);
				
				TArray<AActor*> DamagedResults;
				for (const FAbleQueryResult QueryResult : Results)
				{
					TWeakObjectPtr<AActor> QueryActor = QueryResult.Actor;
					if (!QueryActor.IsValid()) continue;
					if (!DamagedResults.IsEmpty() && DamagedResults.Contains(QueryActor)) continue;
					if (const ISPActorInterface* _ = Cast<ISPActorInterface>(QueryActor))
					{
						DamagedResults.Add(QueryActor.Get());
					}
					if (QueryActor->IsA(ASPGameCharacterBase::StaticClass()))
					{
						ASPGameCharacterBase* Character = Cast<ASPGameCharacterBase>(QueryActor);
						if(IsValid(Character))
						{
							Character->TryPerfectDodge(Context->GetInstigator(), DamageId);
						}
					}
				}

				USPGameLibrary::DoDamageTo(GetWorld(), AbilityId, DamageId, AbilityUniqueID, DamagedResults, FShootDamageEvent(),
										 InstigatorController,
										 DamageSource, TEXT(""));

				for (AActor* DamagedActor : DamagedResults)
				{
					if (IsValid(DamagedActor))
					{
						DoBuffLogic(Context, DamageSource, DamagedActor);
					}
				}

				// 判断是否跳转Segment
				if (m_bHitAndBranchSegment)
				{
					int32 SegmentIndex = Context->GetSelfAbilityComponent()->FindSegment(Context.Get(), m_SegmentName);
					if (SegmentIndex >= 0)
					{
						Context->GetSelfAbilityComponent()->BranchSegment(Context.Get(), SegmentIndex, FJumpSegmentSetting());
					}
				}
			}
		}
	}
}

void USPSkillCollisionQueryDamageTask::DoBuffLogic(const TWeakObjectPtr<const UAbleAbilityContext>& Context,AActor* SourceActor,AActor* TargetActor) const
{
	if(TargetActor == nullptr || !m_AddBuffWhenDamage)
		return;

	if(!m_AddBuffRepeat)
	{
		//Buff只加一次
		USPSkillCollisionQueryDamageTaskScratchPad* ScratchPad = Cast<USPSkillCollisionQueryDamageTaskScratchPad>(
			Context->GetScratchPadForTask(this));
		if(!ScratchPad)
			return;
		if (ScratchPad->AddBuffActors.Contains(TargetActor))
			return;
		bool AddSuccess = USPGameLibrary::AddBuffByDamageSkill(Context->GetAbilityId(),m_BuffID,m_BuffLayer,m_IndexFromSkillCfg,SourceActor,TargetActor,GetWorld());
		if(AddSuccess)
		{
			ScratchPad->AddBuffActors.Add(TargetActor);
		}
	}
	else
	{
		USPGameLibrary::AddBuffByDamageSkill(Context->GetAbilityId(),m_BuffID,m_BuffLayer,m_IndexFromSkillCfg,SourceActor,TargetActor,GetWorld());
	}
}

EAbleAbilityTaskRealm USPSkillCollisionQueryDamageTask::GetTaskRealmBP_Implementation() const { return m_TaskRealm; }

bool USPSkillCollisionQueryDamageTask::IsSingleFrameBP_Implementation() const { return false; }

#undef LOCTEXT_NAMESPACE

// Copyright (c) Extra Life Studios, LLC. All rights reserved.

#include "Game/SPGame/Skill/Task/SPSkillDamageEventTask.h"

#include "ableAbility.h"
#include "ableAbilityUtilities.h"
#include "ableSettings.h"
#include "Async/Async.h"
#include "Game/SPGame/Skill/Core/SPAbilityFunctionLibrary.h"
#include "GameFramework/DamageType.h"
#include "Game/SPGame/Utils/SPGameLibrary.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

USPSkillDamageEventTask::USPSkillDamageEventTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	  m_Damage(1.0f),
	  m_DamageSource(EAbleAbilityTargetType::ATT_Self),
	  m_DamageIndex(0),
	  m_EventName(NAME_None),
	  m_TaskRealm(EAbleAbilityTaskRealm::ATR_ClientAndServer), m_UseAsyncCalculate(false)
{
}

USPSkillDamageEventTask::~USPSkillDamageEventTask()
{
}


void USPSkillDamageEventTask::DoGenericEvent(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
                                             TArray<TWeakObjectPtr<AActor>>& DamageTargets, TArray<float>& DamageValues,
                                             int32 SkillId, int DamageId, AController*& InstigatorController,
                                             AActor*& DamageSource) const
{
	for (int32 i = 0; i < DamageTargets.Num(); ++i)
	{
		if (DamageTargets[i].IsValid())
		{
#if !(UE_BUILD_SHIPPING)
			if (IsVerbose())
			{
				PrintVerbose(Context, FString::Printf(
					             TEXT("Applying %4.2f damage to %s."), DamageValues[i],
					             *DamageTargets[i]->GetName()));
			}
#endif
			const int32 SkillUniqueID = Context.IsValid() ? Context->GetAbilityUniqueID() : 0;
			USPGameLibrary::DoDamage(SkillId, DamageId, SkillUniqueID, DamageTargets[i].Get(), FShootDamageEvent(),
			                         InstigatorController, DamageSource, TEXT(""));
		}
	}
}

void USPSkillDamageEventTask::SetInstigatorController(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
                                                      AController*& InstigatorController) const
{
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
}

void USPSkillDamageEventTask::DoPointDamageEvent(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
                                                 TArray<TWeakObjectPtr<AActor>>& DamageTargets, AActor*& DamageSource,
                                                 TArray<float>& DamageValues, AController*& InstigatorController,
                                                 FVector& ShotDirection, FPointDamageEvent& PointEvent, int32 SkillId,
                                                 int DamageId) const
{
	for (int32 i = 0; i < DamageTargets.Num(); ++i)
	{
		if (DamageTargets[i].IsValid())
		{
			if (DamageSource != nullptr)
			{
				ShotDirection = DamageTargets[i]->GetActorLocation() - DamageSource->GetActorLocation();
				ShotDirection.Normalize();
				PointEvent.ShotDirection = ShotDirection;
			}

			PointEvent.Damage = DamageValues[i];

#if !(UE_BUILD_SHIPPING)
			if (IsVerbose())
			{
				PrintVerbose(Context, FString::Printf(
					             TEXT(
						             "Applying %4.2f damage to %s as Point Damage, Damage Source[%s], Shot Direction [%s]."),
					             DamageValues[i], *DamageTargets[i]->GetName(),
					             DamageSource ? *DamageSource->GetName() : TEXT("NULL"),
					             *PointEvent.ShotDirection.ToString()));
			}
#endif
			const int32 SkillUniqueID = Context.IsValid() ? Context->GetAbilityUniqueID() : 0;
			USPGameLibrary::DoDamage(SkillId, DamageId, SkillUniqueID, DamageTargets[i].Get(), FShootDamageEvent(),
			                         InstigatorController, DamageSource, TEXT(""));
		}
	}
}

void USPSkillDamageEventTask::DoRadialDamageEvent(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
                                                  TArray<TWeakObjectPtr<AActor>>& DamageTargets, AActor*& DamageSource,
                                                  TArray<float>& DamageValues, AController*& InstigatorController,
                                                  FRadialDamageEvent& RadialEvent) const
{
	if (DamageSource != nullptr)
	{
		RadialEvent.Origin = DamageSource->GetActorLocation();
	}
	RadialEvent.Params = ABL_GET_DYNAMIC_PROPERTY_VALUE(Context, m_RadialParams);
	for (int32 i = 0; i < DamageTargets.Num(); ++i)
	{
		if (DamageTargets[i].IsValid())
		{
#if !(UE_BUILD_SHIPPING)
			if (IsVerbose())
			{
				PrintVerbose(Context, FString::Printf(
					             TEXT(
						             "Applying %4.2f damage to %s as Radial Damage, Damage Source[%s], Damage Origin [%s]."),
					             DamageValues[i], *DamageTargets[i]->GetName(),
					             DamageSource ? *DamageSource->GetName() : TEXT("NULL"),
					             *RadialEvent.Origin.ToString()));
			}
#endif

			DamageTargets[i]->TakeDamage(DamageValues[i], *(FDamageEvent*)&RadialEvent,
			                             InstigatorController, DamageSource);
		}
	}
}

void USPSkillDamageEventTask::AddDamageValues(TArray<TFuture<float>>& CalculateTasks, TArray<float>& DamageValues) const
{
	for (TFuture<float>& Future : CalculateTasks)
	{
		if (!Future.IsReady())
		{
			static const FTimespan OneMillisecond = FTimespan::FromMilliseconds(1.0);
			Future.WaitFor(OneMillisecond);
		}

		// Copy the Damage over.
		DamageValues.Add(Future.Get());
	}
}

FString USPSkillDamageEventTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPSkillDamageEventTask");
}

void USPSkillDamageEventTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	check(Context.IsValid());

	Super::OnTaskStart(Context);
	OnTaskStartBP(Context.Get());
}

void USPSkillDamageEventTask::OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const
{

	AActor* DamageSource = GetSingleActorFromTargetType(Context, m_DamageSource);

	TArray<TWeakObjectPtr<AActor>> DamageTargets;
	GetDamageTargets(Context, DamageTargets);

#if !(UE_BUILD_SHIPPING)
	if (IsVerbose())
	{
		PrintVerbose(
			Context, FString::Printf(TEXT("Executing Damage calculations for %d targets."), DamageTargets.Num()));
	}
#endif

	if (DamageTargets.Num())
	{
		TArray<float> DamageValues;
		DamageValues.Reserve(DamageTargets.Num());

		if (m_UseAsyncCalculate && USPAbleSettings::IsAsyncEnabled())
		{
			TArray<TFuture<float>> CalculateTasks;
			CalculateTasks.Reserve(DamageTargets.Num());

			for (TWeakObjectPtr<AActor>& DamageTarget : DamageTargets)
			{
				// Copy over to local variables for the Lambda.
				float BaseDamage = m_Damage;
				FName EventName = m_EventName;

				CalculateTasks.Add(Async(EAsyncExecution::TaskGraph, [&Context, BaseDamage, EventName, &DamageTarget]
				{
					if (Context)
					{
						return Context->GetAbility()->CalculateDamageForActorBP(
							Context, EventName, BaseDamage, DamageTarget.Get());
					}

					return BaseDamage;
				}));
			}

			AddDamageValues(CalculateTasks, DamageValues);
		}
		else
		{
			for (TWeakObjectPtr<AActor>& DamageTarget : DamageTargets)
			{
				DamageValues.Add(
					Context->GetAbility()->CalculateDamageForActorBP(Context, m_EventName, m_Damage,
					                                                 DamageTarget.Get()));

#if !(UE_BUILD_SHIPPING)
				if (IsVerbose() && DamageTarget.IsValid())
				{
					PrintVerbose(Context, FString::Printf(
						             TEXT(
							             "CalculateDamageForActor with Actor %s and Base Damage %4.2f returned %4.2f."),
						             *DamageTarget->GetName(), m_Damage, DamageValues[DamageValues.Num() - 1]));
				}
#endif
			}
		}

		check(DamageTargets.Num() == DamageValues.Num());

		FDamageEvent EmptyEvent;
		EmptyEvent.DamageTypeClass = m_DamageClass;

		int32 DamageIndex = m_DamageIndex;
		int32 SkillId = Context->GetAbilityId();
		FSPSkillInfo SkillInfo;
		USPAbilityFunctionLibrary::GetAbilityInfo(Context->GetWorld(), SkillId, SkillInfo);
		TArray<int32> DamageIds = SkillInfo.DamageIds;
		int DamageId = DamageIds.IsValidIndex(DamageIndex) ? DamageIds[DamageIndex] : INDEX_NONE;
		AController* InstigatorController = nullptr;

		SetInstigatorController(Context, InstigatorController);
		if (EmptyEvent.DamageTypeClass)
		{
			if (EmptyEvent.IsOfType(FPointDamageEvent::ClassID))
			{
				FPointDamageEvent PointEvent;
				FVector ShotDirection;

				DoPointDamageEvent(Context, DamageTargets, DamageSource, DamageValues, InstigatorController,
				                   ShotDirection, PointEvent, SkillId, DamageId);
				return;
			}
			else if (EmptyEvent.IsOfType(FRadialDamageEvent::ClassID))
			{
				FRadialDamageEvent RadialEvent;

				DoRadialDamageEvent(Context, DamageTargets, DamageSource, DamageValues, InstigatorController,
				                    RadialEvent);


				return;
			}
		}
		// Generic event / catch-all
		DoGenericEvent(Context, DamageTargets, DamageValues, SkillId, DamageId, InstigatorController, DamageSource);
	}
}

void USPSkillDamageEventTask::BindDynamicDelegates(UAbleAbility* Ability)
{
	ABL_BIND_DYNAMIC_PROPERTY(Ability, m_DamageClass, TEXT("Damage Class"));
	ABL_BIND_DYNAMIC_PROPERTY(Ability, m_RadialParams, TEXT("Radial Damage Info"));
}

TStatId USPSkillDamageEventTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPSkillDamageEventTask, STATGROUP_USPAbility);
}

void USPSkillDamageEventTask::GetDamageTargets(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
                                               TArray<TWeakObjectPtr<AActor>>& OutArray) const
{
	for (const EAbleAbilityTargetType Target : m_DamageTargets)
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

#if WITH_EDITOR

FText USPSkillDamageEventTask::GetDescriptiveTaskName() const
{
	const FText FormatText = LOCTEXT("SPSkillDamageEventTaskFormat", "{0}: {1}");
	FString TargetNames = TEXT("<None>");
	if (m_DamageTargets.Num())
	{
		TargetNames = FAbleSPLogHelper::GetTargetTargetEnumAsString(m_DamageTargets[0]);
		if (m_DamageTargets.Num() > 1)
		{
			TargetNames.Append(FString::Printf(TEXT(", ... (%d Total Targets)"), m_DamageTargets.Num()));
		}
	}
	return FText::FormatOrdered(FormatText, GetTaskName(), FText::FromString(TargetNames));
}

EDataValidationResult USPSkillDamageEventTask::IsTaskDataValid(const UAbleAbility* AbilityContext,
                                                               const FText& AssetName, TArray<FText>& ValidationErrors)
{
	EDataValidationResult result = EDataValidationResult::Valid;

	// Disabling this for now. It seems to cause more confusion than it fixes, but we'll see if people start complaining about not finding targets.
	//if (m_DamageSource == EAbleAbilityTargetType::ATT_TargetActor || m_DamageTargets.Contains(EAbleAbilityTargetType::ATT_TargetActor))
	//{
	//    if (AbilityContext->RequiresTarget())
	//    {
	//        // trust that the ability cannot run unless it has a target, so don't do any dependency validation
	//        if (AbilityContext->GetTargeting() == nullptr)
	//        {
	//            ValidationErrors.Add(FText::Format(LOCTEXT("NoTargeting", "No Targeting method Defined: {0} with RequiresTarget"), AssetName));
	//            result = EDataValidationResult::Invalid;
	//        }
	//    }
	//    else if (AbilityContext->GetTargeting() == nullptr)
	//    {
	//        // if we have a target actor, we should have a dependency task that copies results
	//        bool hasValidDependency = USPSkillAbilityValidation::CheckDependencies(GetTaskDependencies());            
	//        if (!hasValidDependency)
	//        {
	//            ValidationErrors.Add(FText::Format(LOCTEXT("NoQueryDependency", "Trying to use Target Actor but there's no Ability Targeting or Query( with GetCopyResultsToContext )and a Dependency Defined on this Task: {0}. You need one of those conditions to properly use this target."), FText::FromString(AbilityContext->GetDisplayName())));
	//            result = EDataValidationResult::Invalid;
	//        }
	//    }
	//}

	return result;
}

#endif

bool USPSkillDamageEventTask::IsSingleFrameBP_Implementation() const { return true; }

EAbleAbilityTaskRealm USPSkillDamageEventTask::GetTaskRealmBP_Implementation() const { return m_TaskRealm; }

#undef LOCTEXT_NAMESPACE

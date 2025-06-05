// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SPGame/Skill/Task/SPPlayWeaponAnimationTask.h"

#include "ableAbility.h"
#include "ableSubSystem.h"
#include "MoeFeatureSPLog.h"
#include "Core/LogMacrosEx.h"
#include "Game/SPGame/Gameplay/SPActorInterface.h"
#include "Game/SPGame/Monster/SPGameCharInventoryComponent.h"
#include "Game/SPGame/Weapon/SPWeapon.h"

#define LOCTEXT_NAMESPACE "AbleAbilityTask"

USPPlayWeaponAnimationTaskScratchPad::USPPlayWeaponAnimationTaskScratchPad()
{
}

USPPlayWeaponAnimationTaskScratchPad::~USPPlayWeaponAnimationTaskScratchPad()
{
}

FString USPPlayWeaponAnimationTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPPlayWeaponAnimationTask");
}

USPPlayWeaponAnimationTask::USPPlayWeaponAnimationTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

USPPlayWeaponAnimationTask::~USPPlayWeaponAnimationTask()
{
}

void USPPlayWeaponAnimationTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);
	OnTaskStartBP(Context.Get());
}

void USPPlayWeaponAnimationTask::OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const
{
	
	if (!AnimSequence.LoadSynchronous())
	{
		MOE_SP_ABILITY_WARN(TEXT("No Animation set for SPPlayWeaponAnimation in Ability [%s]"),
        					  *(Context->GetAbility()->GetName()));
		return;
	}

	TArray<TWeakObjectPtr<AActor>> TargetArray;
	GetActorsForTask(Context, TargetArray);

	USPPlayWeaponAnimationTaskScratchPad* ScratchPad = CastChecked<USPPlayWeaponAnimationTaskScratchPad>(Context->GetScratchPadForTask(this));
	ScratchPad->SingleNodeSkeletalComponents.Empty();

	for (TWeakObjectPtr<AActor>& Target : TargetArray)
	{
		if (Target.IsValid())
		{
			if(ISPActorInterface* SPCharacter = Cast<ISPActorInterface>(Target))
			{
				if (USPGameCharInventoryComponent* InventoryComponent = SPCharacter->SPGetInventoryComponent())
				{
					TArray<TWeakObjectPtr<ASPWeapon>> WeaponArray;
					GetWeaponActors(Target, WeaponArray);
					for (TWeakObjectPtr<ASPWeapon>& Weapon : WeaponArray)
					{
						if (Weapon.IsValid())
						{
							TArray<USkeletalMeshComponent*> Components;
							Weapon->GetComponents<USkeletalMeshComponent>(Components);
							for (USkeletalMeshComponent* Component : Components)
							{
								if (UAnimInstance* AnimInstance = Component->GetAnimInstance())
								{
									ScratchPad->SingleNodeSkeletalComponents.Add(Component);
									Component->SetUpdateAnimationInEditor(true);
									AnimInstance->PlaySlotAnimationAsDynamicMontage(AnimSequence.LoadSynchronous(), SlotName, BlendInTime, BlendOutTime, PlayRate, bIsLoop ? 0 : NumberOfLoops, -1, StartTime);
								}
							}
						}
					}
				}
			}
		}
	}
}

void USPPlayWeaponAnimationTask::GetWeaponActors(const TWeakObjectPtr<AActor> Owner,
	TArray<TWeakObjectPtr<ASPWeapon>>& OutWeaponArray) const
{
	if (Owner.IsValid())
	{
		TArray<AActor*> AttachedActors;
		Owner->GetAttachedActors(AttachedActors);
	
		for (AActor* AttachedActor : AttachedActors)
		{
			if (ASPWeapon* SPWeapon = Cast<ASPWeapon>(AttachedActor))
			{
				if (!WeaponName.IsEmpty())
				{
					FString NameString = SPWeapon->GetName();
					if (!NameString.Contains(WeaponName))
					{
						continue;
					}
				}
				OutWeaponArray.AddUnique(SPWeapon);
			}
		}
		if (OutWeaponArray.Num() == 0)
		{
			// 武器跟随模式时，武器并不会Attach到角色身上
			if (AMoeGameCharacter* Character = Cast<AMoeGameCharacter>(Owner.Get()))
			{
				if (ASPWeapon* SPWeapon = Cast<ASPWeapon>(Character->GetCrtEquipWeapon()))
				{
					OutWeaponArray.Add(SPWeapon);
				}
			}
		}
	}
}

void USPPlayWeaponAnimationTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	const EAbleAbilityTaskResult Result) const
{
	Super::OnTaskEnd(Context, Result);
	OnTaskEndBP(Context.Get(), Result);
}

TStatId USPPlayWeaponAnimationTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPPlayWeaponAnimationTask, STATGROUP_USPAbility);
}

UAbleAbilityTaskScratchPad* USPPlayWeaponAnimationTask::CreateScratchPad(
	const TWeakObjectPtr<UAbleAbilityContext>& Context) const
{
	if (UAbleAbilityUtilitySubsystem* Subsystem = Context->GetUtilitySubsystem())
	{
		static TSubclassOf<UAbleAbilityTaskScratchPad> ScratchPadClass = USPPlayWeaponAnimationTaskScratchPad::StaticClass();
		return Subsystem->FindOrConstructTaskScratchPad(ScratchPadClass);
	}

	return NewObject<USPPlayWeaponAnimationTaskScratchPad>(Context.Get());
}

void USPPlayWeaponAnimationTask::OnTaskEndBP_Implementation(const UAbleAbilityContext* Context, const EAbleAbilityTaskResult Result) const
{
	if (!Context)
	{
		return;
	}

	USPPlayWeaponAnimationTaskScratchPad* ScratchPad = CastChecked<USPPlayWeaponAnimationTaskScratchPad>(Context->GetScratchPadForTask(this));
	if (!ScratchPad) return;
	
	if ((Result == EAbleAbilityTaskResult::Interrupted && bStopAtInterrupt) || (Result == EAbleAbilityTaskResult::BranchSegment && bStopAtBranch))
	{
		for (const TWeakObjectPtr<USkeletalMeshComponent>& SkeletalComponent : ScratchPad->SingleNodeSkeletalComponents)
		{
			if (SkeletalComponent.IsValid())
			{
				if (UAnimInstance* AnimInstance = SkeletalComponent->GetAnimInstance())
				{
					AnimInstance->StopSlotAnimation(BlendOutTime, SlotName);
				}
			}
		}
	}

	ScratchPad->SingleNodeSkeletalComponents.Empty();
}

#if WITH_EDITOR
FText USPPlayWeaponAnimationTask::GetDescriptiveTaskName() const
{
	const FText FormatText = LOCTEXT("SPPlayWeaponAnimationTask", "SPPlayWeaponAnimationTask (Animation: {0}");
	const FString AnimSequenceName = AnimSequence.ToSoftObjectPath().ToString();;
	return FText::FormatOrdered(FormatText, FText::FromString(AnimSequenceName));
}
#endif

#undef LOCTEXT_NAMESPACE
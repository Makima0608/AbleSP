// Copyright (c) Extra Life Studios, LLC. All rights reserved.

#include "Game/SPGame/Skill/Task/SPSkillAddImpulseTask.h"
#include "Game/SPGame/Character/SPGameMonsterBase.h"
#include "Game/SPGame/Character/SPGameCharacterBase.h"

#include "MoeFeatureSPLog.h"
#include "Game/SPGame/Utils/SPGameLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

USPSkillAddImpulseTask::USPSkillAddImpulseTask(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer),
	m_Source(ATT_Instigator),
	m_Target(ATT_Owner)
{
}

USPSkillAddImpulseTask::~USPSkillAddImpulseTask()
{
}

FString USPSkillAddImpulseTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPSkillAddImpulseTask");
}

void USPSkillAddImpulseTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);
	OnTaskStartBP(Context.Get());
}

void USPSkillAddImpulseTask::OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const
{

	const int32 RetStepPoise = Context->GetIntParameter(TEXT("RetStepPoise"));
	if (RetStepPoise < 0 && m_RetStepPoise)
	{
		MOE_SP_ABILITY_LOG(TEXT("[SPAbility] USPSkillAddImpulseTask::OnTaskStart [%d] but RetStepPoise < 0 !"), Context->GetAbilityId())
		return;
	}

	AActor* SourceActor = GetSingleActorFromTargetType(Context, m_Source);
	AActor* TargetActor = GetSingleActorFromTargetType(Context, m_Target);
	//死了就不用再addimpulse
	ASPGameMonsterBase* MonsterCharacter = Cast<ASPGameMonsterBase>(TargetActor);
	if (IsValid(MonsterCharacter) && MonsterCharacter->GetCharIsDead())
	{
		return;
	}
	ASPGameCharacterBase* PlayerCharacter = Cast<ASPGameCharacterBase>(TargetActor);
	if (IsValid(PlayerCharacter) && (PlayerCharacter->GetCharIsDead() || PlayerCharacter->IsDying()))
	{
		return;
	}
	bool Ret = false;
	if (IsValid(SourceActor) && IsValid(TargetActor))
	{
		FVector Direction = (TargetActor->GetActorLocation() - SourceActor->GetActorLocation()).GetSafeNormal();
		Direction.Z = 1.f;
		Ret = AddImpulseTo(TargetActor, Direction);
	}
	else if (IsValid(TargetActor))
	{
		Ret = AddImpulseTo(TargetActor, FVector(1.f, 1.f, 1.f));
	}
	
	if (!Ret)
	{
		UE_LOG(LogTemp, Warning, TEXT("USPSkillAddImpulseTask Add Impulse Failed !"));
	}
}

bool USPSkillAddImpulseTask::AddImpulseTo(const AActor* Target, FVector ImpulseDirection) const
{
	UActorComponent* MovementComponent = Target->GetComponentByClass(
		UCharacterMovementComponent::StaticClass());
	if (MovementComponent)
	{
		if (UCharacterMovementComponent* CharacterMovementComponent = Cast<UCharacterMovementComponent>(
			MovementComponent))
		{
			if (CharacterMovementComponent->GetCharacterOwner())
			{
				// ImpulseDirection.Normalize();
				CharacterMovementComponent->AddImpulse(ImpulseDirection * m_ImpulseVector, true);
				return true;
			}
		}
	}

	return false;
}

void USPSkillAddImpulseTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
                                       const EAbleAbilityTaskResult result) const
{
	Super::OnTaskEnd(Context, result);
	OnTaskEndBP(Context.Get(), result);
}

void USPSkillAddImpulseTask::OnTaskEndBP_Implementation(const UAbleAbilityContext* Context,
                                                        const EAbleAbilityTaskResult result) const
{
}

void USPSkillAddImpulseTask::BindDynamicDelegates(UAbleAbility* Ability)
{
	Super::BindDynamicDelegates(Ability);
}

TStatId USPSkillAddImpulseTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPSkillAddImpulseTask, STATGROUP_USPAbility);
}

bool USPSkillAddImpulseTask::IsSingleFrameBP_Implementation() const { return true; }

EAbleAbilityTaskRealm USPSkillAddImpulseTask::GetTaskRealmBP_Implementation() const { return EAbleAbilityTaskRealm::ATR_ClientAndServer; }

#undef LOCTEXT_NAMESPACE

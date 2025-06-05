// Copyright (c) Extra Life Studios, LLC. All rights reserved.

#include "Game/SPGame/Skill/Task/SPSetSkillStageTask.h"

#include "ableAbility.h"
#include "Game/SPGame/Utils/SPGameLibrary.h"

#include "Game/SPGame/Monster/AnimInstance/SPMonsterAnimInstance.h"

#include "Animation/AnimSequenceBase.h"
#include "Animation/AnimSequence.h"
#include "Game/SPGame/Character/SPCharacterMovementComponent.h"
#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

USPSetSkillStageTask::USPSetSkillStageTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	  , Stage(ESPSkillStage::Start), Last()
{
}

USPSetSkillStageTask::~USPSetSkillStageTask()
{
}

FString USPSetSkillStageTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPSetSkillStageTask");
}

void USPSetSkillStageTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);
	OnTaskStartBP(Context.Get());
}

void USPSetSkillStageTask::OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const
{

	ESPSkillStage AnimStage = Stage;
	if (AnimStage >= ESPSkillStage::Max) return;

	const AActor* Target = Context->GetOwner();
	if (!IsValid(Target)) return;

	const USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(Target->GetComponentByClass(
		USkeletalMeshComponent::StaticClass()));

	if (!SkeletalMeshComponent) return;

	UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance();
	if (!AnimInstance || !AnimInstance->IsA(USPMonsterAnimInstance::StaticClass())) return;
	USPMonsterAnimInstance* MonsterAnimInstance = Cast<USPMonsterAnimInstance>(AnimInstance);
	if (MonsterAnimInstance)
	{
		MonsterAnimInstance->SetSkillAnimStage(AnimStage);
		MonsterAnimInstance->SetSkillType(ESPSkillType::FarSkill);
		if(AnimStage == ESPSkillStage::Start)
		{
			SetupSkillStartParam(MonsterAnimInstance);
		}
	}
	
	if (m_EnableAnimMove)
	{
		AActor* Actor = Context->GetSelfActor();
		if (IsValid(Actor))
		{
			USPCharacterMovementComponent* MovementComponent = Actor->FindComponentByClass<USPCharacterMovementComponent>();
			if (IsValid(MovementComponent))
			{
				MovementComponent->SetShouldAnimMove(true, FVector::ZeroVector, m_CloseFriction);
			}
		}
	}
}

void USPSetSkillStageTask::SetupSkillStartParam(USPMonsterAnimInstance* AnimInstance) const
{
	if(AnimInstance == nullptr)
		return;
	const float ActionPreAnimTime = GetSequencePlayLength(AnimInstance,"A_FarSkill_Action");
	const float DurationTime = this->GetDuration();
	const float SkillStartAnimLeftTime =FMath::Clamp(DurationTime - ActionPreAnimTime,0.f,DurationTime);
	AnimInstance->SetSkillStartLoopAnimLeftTime(SkillStartAnimLeftTime);
}

float USPSetSkillStageTask::GetSequencePlayLength(const USPMonsterAnimInstance* AnimInstance,const FString& PropertyName)
{
	if(AnimInstance== nullptr || PropertyName.IsEmpty())
		return 0;

	FObjectProperty* AnimSequenceProperty = static_cast<FObjectProperty*>(FindFProperty<FProperty>(AnimInstance->GetClass(),*PropertyName));
	if(AnimSequenceProperty == nullptr)
		return 0;

	UAnimSequenceBase* TargetSequence =  Cast<UAnimSequenceBase>(AnimSequenceProperty->GetObjectPropertyValue_InContainer(AnimInstance));
	if(TargetSequence == nullptr)
		return 0;
	
	return TargetSequence->GetPlayLength();
}

void USPSetSkillStageTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
                                     const EAbleAbilityTaskResult result) const
{
	Super::OnTaskEnd(Context, result);
	OnTaskEndBP(Context.Get(), result);
}

void USPSetSkillStageTask::OnTaskEndBP_Implementation(const UAbleAbilityContext* Context,
                                                      const EAbleAbilityTaskResult result) const
{
	
	// todo...目前没找到在技能结束时统一写后处理地方，部分回退逻辑无法很好的实现，以及判断分段式task在最后做收尾处理
	// todo...先临时手配一个字段来进行标识 @bladeyuan
	if (Last || result == Interrupted)
	{
		const AActor* Target = Context->GetOwner();
		if (!IsValid(Target)) return;

		const USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(Target->GetComponentByClass(
			USkeletalMeshComponent::StaticClass()));

		if (!SkeletalMeshComponent) return;

		UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance();
		if (!AnimInstance || !AnimInstance->IsA(USPMonsterAnimInstance::StaticClass())) return;
		USPMonsterAnimInstance* MonsterAnimInstance = Cast<USPMonsterAnimInstance>(AnimInstance);
		if (MonsterAnimInstance)
			MonsterAnimInstance->SetSkillType(ESPSkillType::Normal);

		if (m_EnableAnimMove)
		{
			AActor* Actor = Context->GetSelfActor();
			if (IsValid(Actor))
			{
				USPCharacterMovementComponent* MovementComponent = Actor->FindComponentByClass<USPCharacterMovementComponent>();
				if (IsValid(MovementComponent))
				{
					MovementComponent->SetShouldAnimMove(false, FVector::ZeroVector, m_CloseFriction);
				}
			}
		}
	}
}

TStatId USPSetSkillStageTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPSetSkillStageTask, STATGROUP_USPAbility);
}

void USPSetSkillStageTask::BindDynamicDelegates(UAbleAbility* Ability)
{
	Super::BindDynamicDelegates(Ability);
	ABL_BIND_DYNAMIC_PROPERTY(Ability, Stage, TEXT("Skill Stage"));
}

bool USPSetSkillStageTask::IsSingleFrameBP_Implementation() const { return false; }

EAbleAbilityTaskRealm USPSetSkillStageTask::GetTaskRealmBP_Implementation() const	
{
	return EAbleAbilityTaskRealm::ATR_ClientAndServer;
}

#undef LOCTEXT_NAMESPACE

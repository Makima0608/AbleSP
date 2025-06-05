// Copyright (c) Extra Life Studios, LLC. All rights reserved.

#include "Game/SPGame/Skill/Task/SPSkillChargeTask.h"

#include "ableSubSystem.h"
#include "Game/SPGame/Utils/SPGameLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

USPChargeTaskScratchPad::USPChargeTaskScratchPad()
{
}

USPChargeTaskScratchPad::~USPChargeTaskScratchPad()
{
}

USPSkillChargeTask::USPSkillChargeTask(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer),
	ChargeCurve(nullptr)
{
}

USPSkillChargeTask::~USPSkillChargeTask()
{
}

FString USPSkillChargeTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPSkillChargeTask");
}

void USPSkillChargeTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);
	OnTaskStartBP(Context.Get());
}

void USPSkillChargeTask::OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const
{
}

void USPSkillChargeTask::OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context, float deltaTime) const
{
	Super::OnTaskTick(Context, deltaTime);
	OnTaskTickBP(Context.Get(), deltaTime);
}

void USPSkillChargeTask::OnTaskTickBP_Implementation(const UAbleAbilityContext* Context, float deltaTime) const
{

	USPChargeTaskScratchPad* ScratchPad = CastChecked<USPChargeTaskScratchPad>(Context->GetScratchPadForTask(this));
	if (!ScratchPad) return;

	const float Duration = GetDuration();
	const float TimePercent = ScratchPad->ChargedTime / (Duration > 0.f ? Duration : 1.f);
	if (ChargeCurve.LoadSynchronous())
	{
		const float ImpulseForcePercent = ChargeCurve->GetFloatValue(TimePercent);
		const AActor* Target = GetSingleActorFromTargetType(Context, m_Target);
		if (!IsValid(Target))
		{
			UE_LOG(LogTemp, Log, TEXT("AbilityChargeTask Failed, Invalid Target !"));
			return;
		}
		
		const FVector ForwardVector = Target->GetActorForwardVector();
		const FVector RightVector = Target->GetActorRightVector();

		const FVector ImpulseForce = ForwardVector * ImpulseForcePercent * m_MaxImpulse;
		auto bRet = AddImpulse(Target, ImpulseForce);
	}

	ScratchPad->ChargedTime += deltaTime;
}

bool USPSkillChargeTask::AddImpulse(const AActor* Target, const FVector ImpulseForce) const
{
	UActorComponent* MovementComponent = Target->GetComponentByClass(
		UCharacterMovementComponent::StaticClass());
	if (MovementComponent)
	{
		if (UCharacterMovementComponent* CharacterMovementComponent = Cast<UCharacterMovementComponent>(
			MovementComponent))
		{
			CharacterMovementComponent->AddImpulse(ImpulseForce, true);
			return true;
		}
	}

	return false;
}

void USPSkillChargeTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
                                   const EAbleAbilityTaskResult result) const
{
	Super::OnTaskEnd(Context, result);
	OnTaskEndBP(Context.Get(), result);
}

void USPSkillChargeTask::OnTaskEndBP_Implementation(const UAbleAbilityContext* Context,
                                                    const EAbleAbilityTaskResult result) const
{
	USPChargeTaskScratchPad* ScratchPad = CastChecked<USPChargeTaskScratchPad>(Context->GetScratchPadForTask(this));
	if (!ScratchPad) return;

	ScratchPad->ChargedTime = .0f;
}

void USPSkillChargeTask::BindDynamicDelegates(UAbleAbility* Ability)
{
	Super::BindDynamicDelegates(Ability);
}

TStatId USPSkillChargeTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPSkillChargeTask, STATGROUP_USPAbility);
}

UAbleAbilityTaskScratchPad* USPSkillChargeTask::CreateScratchPad(
	const TWeakObjectPtr<UAbleAbilityContext>& Context) const
{
	if (UAbleAbilityUtilitySubsystem* Subsystem = Context->GetUtilitySubsystem())
	{
		static TSubclassOf<UAbleAbilityTaskScratchPad> ScratchPadClass = USPChargeTaskScratchPad::StaticClass();
		return Subsystem->FindOrConstructTaskScratchPad(ScratchPadClass);
	}

	return NewObject<USPChargeTaskScratchPad>(Context.Get());
}

bool USPSkillChargeTask::IsSingleFrameBP_Implementation() const { return false; }

EAbleAbilityTaskRealm USPSkillChargeTask::GetTaskRealmBP_Implementation() const { return EAbleAbilityTaskRealm::ATR_ClientAndServer; }

#undef LOCTEXT_NAMESPACE

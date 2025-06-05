// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SPGame/Skill/Task/SPAdjustGravityScaleTask.h"

#include "ableSubSystem.h"
#include "DrawDebugHelpers.h"
#include "MoeFeatureSPLog.h"
#include "Core/LogMacrosEx.h"
#include "Game/SPGame/Character/SPCharacterMovementComponent.h"

USPAdjustGravityScaleTaskScratchPad::USPAdjustGravityScaleTaskScratchPad()
{
}

USPAdjustGravityScaleTaskScratchPad::~USPAdjustGravityScaleTaskScratchPad()
{
}

void USPAdjustGravityScaleTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);
	OnTaskStartBP(Context.Get());
}

void USPAdjustGravityScaleTask::OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const
{
	MOE_SP_ABILITY_LOG(TEXT("USPAdjustGravityScaleTask::OnTaskStartBP"));

	USPAdjustGravityScaleTaskScratchPad* ScratchPad = Cast<USPAdjustGravityScaleTaskScratchPad>(
	Context->GetScratchPadForTask(this));
	if (!ScratchPad) return;
	ScratchPad->DuringTime = 0.0f;
	float CurveValue = GravityCurve->GetFloatValue(ScratchPad->DuringTime);
	AActor* Actor = Context->GetSelfActor();
	if (IsValid(Actor))
	{
		USPCharacterMovementComponent* MovementComponent = Actor->FindComponentByClass<USPCharacterMovementComponent>();
		if (IsValid(MovementComponent))
		{
			if (bIsSetVelocitZtoZero)
			{
				MovementComponent->Velocity.Z = 0.0f;
			}
			MovementComponent->SetVariableGravityScale(CurveValue, TEXT("USPAdjustGravityScaleTask::OnTaskStartBP"));
			MovementComponent->SetIsDuringAdjustGravity(true);

			MOE_SP_ABILITY_LOG(TEXT("USPAdjustGravityScaleTask::OnTaskStartBP SetIsDuringAdjustGravity true"));

			// DrawDebugPoint(Actor->GetWorld(), MovementComponent->GetActorLocation(), 25, FColor::Green, false, 10.0);
			// MOE_LOG(LogTemp, Warning, TEXT("burger:: Velocity.Z : %f at time %f"), MovementComponent->Velocity.Z, ScratchPad->DuringTime)
		}
	}
}

void USPAdjustGravityScaleTask::OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	float deltaTime) const
{
	Super::OnTaskTick(Context, deltaTime);
	OnTaskTickBP(const_cast<UAbleAbilityContext*>(Context.Get()), deltaTime);
}

void USPAdjustGravityScaleTask::OnTaskTickBP_Implementation(const UAbleAbilityContext* Context, float deltaTime) const
{
	USPAdjustGravityScaleTaskScratchPad* ScratchPad = Cast<USPAdjustGravityScaleTaskScratchPad>(
	Context->GetScratchPadForTask(this));
	if (!ScratchPad) return;
	ScratchPad->DuringTime += deltaTime;
	float CurveValue = GravityCurve->GetFloatValue(ScratchPad->DuringTime);
	AActor* Actor = Context->GetSelfActor();
	if (IsValid(Actor))
	{
		USPCharacterMovementComponent* MovementComponent = Actor->FindComponentByClass<USPCharacterMovementComponent>();
		if (IsValid(MovementComponent))
		{
			MovementComponent->SetVariableGravityScale(CurveValue, TEXT("USPAdjustGravityScaleTask::OnTaskTickBP"));
			
			// DrawDebugPoint(Actor->GetWorld(), MovementComponent->GetActorLocation(), 25, FColor::Red, false, 10.0);
			// MOE_LOG(LogTemp, Warning, TEXT("burger:: Velocity.Z : %f at time %f"), MovementComponent->Velocity.Z, ScratchPad->DuringTime)
		}
	}
}

void USPAdjustGravityScaleTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	const EAbleAbilityTaskResult result) const
{
	OnTaskEndBP(const_cast<UAbleAbilityContext*>(Context.Get()), result);
	Super::OnTaskEnd(Context, result);
}

void USPAdjustGravityScaleTask::OnTaskEndBP_Implementation(const UAbleAbilityContext* Context,
                                                           const EAbleAbilityTaskResult result) const
{
	MOE_SP_ABILITY_LOG(TEXT("USPAdjustGravityScaleTask::OnTaskEndBP"));

	AActor* Actor = Context->GetSelfActor();
	if (IsValid(Actor))
	{
		USPCharacterMovementComponent* MovementComponent = Actor->FindComponentByClass<USPCharacterMovementComponent>();
		if (IsValid(MovementComponent))
		{
			MovementComponent->SetVariableGravityScale(1.0, TEXT("USPAdjustGravityScaleTask::OnTaskEndBP"));
			MovementComponent->SetIsDuringAdjustGravity(false);
			
			MOE_SP_ABILITY_LOG(TEXT("USPAdjustGravityScaleTask::OnTaskStartBP SetIsDuringAdjustGravity false"));
		}
	}
}

TStatId USPAdjustGravityScaleTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPAdjustGravityScaleTask, STATGROUP_USPAbility);
}

EAbleAbilityTaskRealm USPAdjustGravityScaleTask::GetTaskRealm() const
{
	return EAbleAbilityTaskRealm::ATR_ClientAndServer;
}

UAbleAbilityTaskScratchPad* USPAdjustGravityScaleTask::CreateScratchPad(
	const TWeakObjectPtr<UAbleAbilityContext>& Context) const
{
	if (UAbleAbilityUtilitySubsystem* Subsystem = Context->GetUtilitySubsystem())
	{
		static TSubclassOf<UAbleAbilityTaskScratchPad> ScratchPadClass =
			USPAdjustGravityScaleTaskScratchPad::StaticClass();
		return Subsystem->FindOrConstructTaskScratchPad(ScratchPadClass);
	}

	return NewObject<USPAdjustGravityScaleTaskScratchPad>(Context.Get());
}





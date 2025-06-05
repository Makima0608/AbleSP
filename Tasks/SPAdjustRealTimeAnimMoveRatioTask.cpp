// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SPGame/Skill/Task/SPAdjustRealTimeAnimMoveRatioTask.h"

#include "ableSubSystem.h"
#include "Game/SPGame/Character/SPCharacterMovementComponent.h"
#include "Game/SPGame/Utils/SPGameLibrary.h"

USPAdjustRealTimeAnimMoveRatioTaskScratchPad::USPAdjustRealTimeAnimMoveRatioTaskScratchPad()
{
}

USPAdjustRealTimeAnimMoveRatioTaskScratchPad::~USPAdjustRealTimeAnimMoveRatioTaskScratchPad()
{
}

void USPAdjustRealTimeAnimMoveRatioTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);
	OnTaskStartBP(Context.Get());
}

void USPAdjustRealTimeAnimMoveRatioTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	const EAbleAbilityTaskResult result) const
{
	OnTaskEndBP(const_cast<UAbleAbilityContext*>(Context.Get()), result);
	Super::OnTaskEnd(Context, result);
}

void USPAdjustRealTimeAnimMoveRatioTask::OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	float deltaTime) const
{
	Super::OnTaskTick(Context, deltaTime);
	OnTaskTickBP(const_cast<UAbleAbilityContext*>(Context.Get()), deltaTime);
}

TStatId USPAdjustRealTimeAnimMoveRatioTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPAdjustRealTimeAnimMoveRatioTask, STATGROUP_USPAbility);
}

UAbleAbilityTaskScratchPad* USPAdjustRealTimeAnimMoveRatioTask::CreateScratchPad(
	const TWeakObjectPtr<UAbleAbilityContext>& Context) const
{
	if (UAbleAbilityUtilitySubsystem* Subsystem = Context->GetUtilitySubsystem())
	{
		static TSubclassOf<UAbleAbilityTaskScratchPad> ScratchPadClass =
			USPAdjustRealTimeAnimMoveRatioTaskScratchPad::StaticClass();
		return Subsystem->FindOrConstructTaskScratchPad(ScratchPadClass);
	}

	return NewObject<USPAdjustRealTimeAnimMoveRatioTaskScratchPad>(Context.Get());
}

void USPAdjustRealTimeAnimMoveRatioTask::OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const
{
	USPAdjustRealTimeAnimMoveRatioTaskScratchPad* ScratchPad = Cast<USPAdjustRealTimeAnimMoveRatioTaskScratchPad>(
		Context->GetScratchPadForTask(this));
	if (!ScratchPad) return;
	ScratchPad->DuringTime = 0.0f;
	AActor* Actor = Context->GetSelfActor();
	if (IsValid(Actor))
	{
		USPCharacterMovementComponent* MovementComponent = Actor->FindComponentByClass<USPCharacterMovementComponent>();
		if (IsValid(MovementComponent))
		{
			if (!MovementComponent->IsDuringAnimMove())
			{
				return;
			}
			
			ScratchPad->CacheAnimMoveRatio = MovementComponent->GetAnimMoveRatio();

			FName CurrentAnimName = MovementComponent->GetCurrentAnimMoveName();
			if (AnimMoveRatioCurve.Contains(CurrentAnimName))
			{
				UCurveFloat* CurrentCurve = AnimMoveRatioCurve[CurrentAnimName];
				if (IsValid(CurrentCurve))
				{
					float Ratio = CurrentCurve->GetFloatValue(ScratchPad->DuringTime);
					MovementComponent->SetAnimMoveRatio(Ratio);
				}
			}
		}
	}
}

void USPAdjustRealTimeAnimMoveRatioTask::OnTaskTickBP_Implementation(const UAbleAbilityContext* Context,
                                                                     float deltaTime) const
{
	USPAdjustRealTimeAnimMoveRatioTaskScratchPad* ScratchPad = Cast<USPAdjustRealTimeAnimMoveRatioTaskScratchPad>(
	Context->GetScratchPadForTask(this));
	if (!ScratchPad) return;
	ScratchPad->DuringTime += deltaTime;
	AActor* Actor = Context->GetSelfActor();
	if (IsValid(Actor))
	{
		USPCharacterMovementComponent* MovementComponent = Actor->FindComponentByClass<USPCharacterMovementComponent>();
		if (IsValid(MovementComponent))
		{
			if (!MovementComponent->IsDuringAnimMove())
			{
				return;
			}
			FName CurrentAnimName = MovementComponent->GetCurrentAnimMoveName();
			if (AnimMoveRatioCurve.Contains(CurrentAnimName))
			{
				UCurveFloat* CurrentCurve = AnimMoveRatioCurve[CurrentAnimName];
				if (IsValid(CurrentCurve))
				{
					float Ratio = CurrentCurve->GetFloatValue(ScratchPad->DuringTime);
					MovementComponent->SetAnimMoveRatio(Ratio);
				}
			}
		}
	}
}

void USPAdjustRealTimeAnimMoveRatioTask::OnTaskEndBP_Implementation(const UAbleAbilityContext* Context,
                                                                    const EAbleAbilityTaskResult result) const
{
	USPAdjustRealTimeAnimMoveRatioTaskScratchPad* ScratchPad = Cast<USPAdjustRealTimeAnimMoveRatioTaskScratchPad>(
	Context->GetScratchPadForTask(this));
	if (!ScratchPad) return;
	AActor* Actor = Context->GetSelfActor();
	if (IsValid(Actor))
	{
		USPCharacterMovementComponent* MovementComponent = Actor->FindComponentByClass<USPCharacterMovementComponent>();
		if (IsValid(MovementComponent))
		{
			if (!MovementComponent->IsDuringAnimMove())
			{
				return;
			}
			
			MovementComponent->SetAnimMoveRatio(ScratchPad->CacheAnimMoveRatio);
		}
	}
}



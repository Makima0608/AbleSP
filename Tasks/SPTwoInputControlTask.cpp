// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SPGame/Skill/Task/SPTwoInputControlTask.h"
#include "MoeFeatureSPLog.h"
#include "Game/SPGame/Character/SPGameCharacterBase.h"

void USPTwoInputControlTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	MOE_SP_ABILITY_LOG(TEXT("USPTwoInputControlTask::OnTaskStart"));
	Super::OnTaskStart(Context);
	OnTaskStartBP(Context.Get());
}

void USPTwoInputControlTask::OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context, float deltaTime) const
{
	Super::OnTaskTick(Context, deltaTime);
	OnTaskTickBP(Context.Get(), deltaTime);
}

void USPTwoInputControlTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,const EAbleAbilityTaskResult result) const
{
	OnTaskEndBP(Context.Get(), result);
	Super::OnTaskEnd(Context, result);
}

void USPTwoInputControlTask::OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const
{
	ASPGameCharacterBase* Player = Cast<ASPGameCharacterBase>(Context->GetOwner());
	if (Player)
	{
		InputComp = Player->FindComponentByClass<USPGameCharInputComponent>();
	}
}

void USPTwoInputControlTask::OnTaskTickBP_Implementation(const UAbleAbilityContext* Context, float deltaTime) const
{
	if (InputComp != nullptr)
	{
		
		const FVector moveInput = InputComp->GetCurrentJoystickInput();
		FVector MoveDir =  moveInput.GetSafeNormal();
		OnInput(Context, MoveDir);
	}
}

void USPTwoInputControlTask::OnTaskEndBP_Implementation(const UAbleAbilityContext* Context, const EAbleAbilityTaskResult result) const
{
}

void USPTwoInputControlTask::OnInput(const TWeakObjectPtr<const UAbleAbilityContext>& Context, FVector Direction) const
{
	float ProgressRate = GetCurrentDurationAsRatio(Context.Get());
	if(FirstInterruptBegin <= FirstInterruptEnd && SecondInterruptBegin <= SecondInterruptEnd)
	{
		if(FMath::IsWithin(ProgressRate, FirstInterruptBegin, FirstInterruptEnd) && !FMath::IsWithin(ProgressRate, SecondInterruptBegin, SecondInterruptEnd))
		{
			if (Direction.Size() > 0)
			{
				ApplyControlSpeed(Context, true);
			
				const UAbleAbility* Ability = Context.Get()->GetAbility();
				USPAbilityComponent* AbleAbilityComp = Cast<USPAbilityComponent>(Context.Get()->GetSelfAbilityComponent());
				if (Ability != nullptr && AbleAbilityComp != nullptr)
				{
					const int32 AbilityId = Context.Get()->GetAbilityId();
					AbleAbilityComp->InterruptedAbility(AbilityId, "USPTwoInputControlTask::OnInput");
					MOE_SP_ABILITY_LOG(TEXT("USPTwoInputControlTask::OnInput InterruptedAbility AbilityId %d"), AbilityId);
				}
			}
		}
		else if (!FMath::IsWithin(ProgressRate, FirstInterruptBegin, FirstInterruptEnd) && FMath::IsWithin(ProgressRate, SecondInterruptBegin, SecondInterruptEnd))
		{
			if (Direction.Size() > 0)
			{
				ApplyControlSpeed(Context, false);

				const UAbleAbility* Ability = Context.Get()->GetAbility();
				USPAbilityComponent* AbleAbilityComp = Cast<USPAbilityComponent>(Context.Get()->GetSelfAbilityComponent());
				if (Ability != nullptr && AbleAbilityComp != nullptr)
				{
					const int32 AbilityId = Context.Get()->GetAbilityId();
					AbleAbilityComp->InterruptedAbility(AbilityId, "USPTwoInputControlTask::OnInput");
					MOE_SP_ABILITY_LOG(TEXT("USPTwoInputControlTask::OnInput InterruptedAbility AbilityId %d"), AbilityId);
				}
			}
		}
		else if (FMath::IsWithin(ProgressRate, FirstInterruptBegin, FirstInterruptEnd) && FMath::IsWithin(ProgressRate, SecondInterruptBegin, SecondInterruptEnd))
		{
			if (Direction.Size() > 0)
			{
				if(FirstPriority >= SecondPriority)
				{
					ApplyControlSpeed(Context, true);
				}else
				{
					ApplyControlSpeed(Context, false);
				}
				const UAbleAbility* Ability = Context.Get()->GetAbility();
				USPAbilityComponent* AbleAbilityComp = Cast<USPAbilityComponent>(Context.Get()->GetSelfAbilityComponent());
				if (Ability != nullptr && AbleAbilityComp != nullptr)
				{
					const int32 AbilityId = Context.Get()->GetAbilityId();
					AbleAbilityComp->InterruptedAbility(AbilityId, "USPTwoInputControlTask::OnInput");
					MOE_SP_ABILITY_LOG(TEXT("USPTwoInputControlTask::OnInput InterruptedAbility AbilityId %d"), AbilityId);
				}
			}
		}
	}
}

void USPTwoInputControlTask::ApplyControlSpeed(const TWeakObjectPtr<const UAbleAbilityContext>& Context, bool IsUseMaxSpeed) const
{
	ASPGameCharacterBase* Player = Cast<ASPGameCharacterBase>(Context->GetOwner());
	if (Player)
	{
		UCharacterMovementComponent* CharMoveComp = Player->GetCharacterMovement();
		if (CharMoveComp != nullptr)
		{
			if(IsUseMaxSpeed)
			{
				InputComp->RespSprintState();
			}
			const FVector moveInput = InputComp->GetCurrentJoystickInput();
			FVector moveDir =  moveInput.GetSafeNormal2D();
			FVector CurVelocity = CharMoveComp->Velocity;
			FVector NewVelocity = moveDir * CharMoveComp->GetMaxSpeed();
			
			CharMoveComp->Velocity = FVector(NewVelocity.X, NewVelocity.Y, CurVelocity.Z);
		}
	}
}

FString USPTwoInputControlTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPTwoInputControlTask");
}


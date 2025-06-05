// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SPGame/Skill/Task/SPStopSprintTask.h"

#include "MoeFeatureSPLog.h"
#include "Game/SPGame/Character/SPCharacterMovementComponent.h"
#include "Game/SPGame/Character/SPGameCharacterBase.h"
#include "Game/SPGame/Character/Animation/SPGameCharacterAnimInstance.h"

void USPStopSprintTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);
	OnTaskStartBP(Context.Get());
}

void USPStopSprintTask::OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const
{
	MOE_SP_ABILITY_LOG(TEXT("USPStopSprintTask::OnTaskStartBP Run"))

	AActor* Actor = Context->GetSelfActor();
	if (IsValid(Actor))
	{
		USPCharacterMovementComponent* MovementComponent = Actor->FindComponentByClass<USPCharacterMovementComponent>();
		if (IsValid(MovementComponent))
		{
			FVector Direction = Context->GetVectorParameter(TEXT("Direction"));
			// bool IsRightFoot = Context->GetIntParameter(TEXT("IsFootRight")) == 1 ? true : false;
			//
			// if (IsRightFoot)
			// {
			// 	MovementComponent->StartPlayAnimMovement("AS_SP_Stop_Sprint_R_TM");
			// }
			// else
			// {
			// 	MovementComponent->StartPlayAnimMovement("AS_SP_Stop_Sprint_L_TM");
			// }

			int32 FootChoice = Context->GetIntParameter(TEXT("IsFootRight"));
			ASPGameCharacterBase* SPChar = Cast<ASPGameCharacterBase>(Actor);
			if (IsValid(SPChar))
			{
				USkeletalMeshComponent* SPMesh = SPChar->GetCharMesh();
				if (IsValid(SPMesh))
				{
					USPGameCharacterAnimInstance* AnimInstance = Cast<USPGameCharacterAnimInstance>(
						SPMesh->GetAnimInstance());
					if (IsValid(AnimInstance))
					{
						UAnimSequenceBase* AnimSequence = USPGameLibrary::GetLinkedAnimInstancesStopSprintAnimSequence(SPMesh, FootChoice);
						if (IsValid(AnimSequence))
						{
							MovementComponent->StartPlayAnimMovement(AnimSequence->GetName());
						}
					}
				}
			}
			
			MovementComponent->SetShouldAnimMove(true, Direction, false, false, false, false, false);
			MovementComponent->SetAnimMoveRatio(1.0f);

		}
	}
}

void USPStopSprintTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	const EAbleAbilityTaskResult result) const
{
	OnTaskEndBP(const_cast<UAbleAbilityContext*>(Context.Get()), result);
	Super::OnTaskEnd(Context, result);
}

void USPStopSprintTask::OnTaskEndBP_Implementation(const UAbleAbilityContext* Context,
												   const EAbleAbilityTaskResult result) const
{
	MOE_SP_ABILITY_LOG(TEXT("USPStopSprintTask::OnTaskEndBP Run"))

	AActor* Actor = Context->GetSelfActor();
	if (IsValid(Actor))
	{
		USPCharacterMovementComponent* MovementComponent = Actor->FindComponentByClass<USPCharacterMovementComponent>();
		if (IsValid(MovementComponent))
		{
			MovementComponent->SetShouldAnimMove(false, FVector::ZeroVector, false);
			
		}
		
		bool IsExitByState = Context->GetIntParameter(TEXT("IsExitByState")) == 1 ? true : false;
		if (!IsExitByState)
		{
			ASPGameCharacterBase* SPChar = Cast<ASPGameCharacterBase>(Actor);
			if (IsValid(SPChar))
			{
				SPChar->TryLeaveMotionState_New(static_cast<uint8>(ESPMotionState::SP_StopSprint), EStateIdOffsetType::SP);
				MOE_SP_ABILITY_LOG(TEXT("USPStopSprintTask::OnTaskEndBP TryLeaveMotionState"))
			}
		}
	}
}

TStatId USPStopSprintTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPStopSprintTask, STATGROUP_USPAbility);
}





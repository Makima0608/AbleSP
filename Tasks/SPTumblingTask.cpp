// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SPGame/Skill/Task/SPTumblingTask.h"
#include "Animation/AnimInstanceProxy.h"
#include "Game/SPGame/Character/SPCharacterMovementComponent.h"

#include "Game/SPGame/Character/SPGameCharacterBase.h"
#include "Game/SPGame/Character/SPGameCharInputComponent.h"
#include "Game/SPGame/Character/SPGameMonsterBase.h"
#include "Game/SPGame/Character/SPPerfectDodgeComponent.h"
#include "Game/SPGame/Character/Animation/SPGameCharacterAnimInstance.h"
#include "Game/SPGame/Skill/Task/SPTumblingTaskScratchPad.h"
#include "Game/SPGame/State/Motion/SPCharMotionStateTumbling.h"
#include "MoeGameplay/Character/Component/MoeCharAttributeComponent.h"

DEFINE_LOG_CATEGORY(LogTumblingTask);
#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

USPTumblingTask::USPTumblingTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

USPTumblingTask::~USPTumblingTask()
{
}


FString USPTumblingTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPTumblingTask");
}

void USPTumblingTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
    OnTaskStartBP_Override(Context.Get());
	UAbleAbilityTask::OnTaskStart(Context);
}

void USPTumblingTask::OnTaskStartBP_Override_Implementation(const UAbleAbilityContext* Context) const
{
    UAnimationAsset* AnimationAsset = m_AnimationAsset;

	if (!AnimationAsset)
	{
		FString DisplayName;
		if (Context->GetAbility() != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("No Animation set for USPTumblingTask in Ability"));
		}
		return;
	}

	const ASPGameCharacterBase* SPChar = Cast<ASPGameCharacterBase>(Context->GetSelfActor());
	if (!IsValid(SPChar))
	{
		return;
	}
	MOE_LOG(LogTumblingTask, Log, TEXT("USPTumblingTask::OnTaskStartBP_Override"));
	SPAnimStateComp = SPChar->FindComponentByClass<USPAnimStateComponent>();
	if (SPAnimStateComp != nullptr)
	{
		SPAnimStateComp->AnimLayerPoseState = EMoeAnimModule_AnimPoseState::Moe_Action_Body;
		SPAnimStateComp->AnimActionState = m_TumblingStateType;
	}
	ChangeCharTumblingSlidingAnim(SPChar, AnimationAsset);
	SetPoseState(SPChar, true);
}

void USPTumblingTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context, const EAbleAbilityTaskResult result) const
{
    OnTaskEndBP_Override(Context.Get(), result);
	UAbleAbilityTask::OnTaskEnd(Context, result);
}

void USPTumblingTask::OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context, float deltaTime) const
{
	Super::OnTaskTick(Context, deltaTime);
	if (SPAnimStateComp != nullptr)
	{
		SPAnimStateComp->TumblingStarPosition = Context->GetCurrentTime();
	}
}

void USPTumblingTask::OnTaskEndBP_Override_Implementation(const UAbleAbilityContext* Context, const EAbleAbilityTaskResult result) const
{
	MOE_LOG(LogTumblingTask, Log, TEXT("USPTumblingTask::OnTaskEndBP_Override"));
    const ASPGameCharacterBase* SPChar = Cast<ASPGameCharacterBase>(Context->GetSelfActor());
	if (!IsValid(SPChar))
	{
		return;
	}
	SetPoseState(SPChar, false);
	ResetPerfectDodge(SPChar);
	//this->ChangeCharTumblingSlidingAnim(SPChar, nullptr);
}

bool USPTumblingTask::ChangeCharTumblingSlidingAnim_Implementation(const ASPGameCharacterBase* SPChar, UAnimationAsset* AnimationAsset) const
{
	if (m_TumblingStateType != EMoeAnimModule_AnimBodyActionState::SP_Tumbling_Sliding)
	{
		return true;
	}

	if (SPAnimStateComp == nullptr)
	{
		return false;
	}
	SPAnimStateComp->TumblingSlidingSequence = Cast<UAnimSequence>(AnimationAsset);
	SPAnimStateComp->bReplaceTumblingSequence = true;
	return true;
}

void USPTumblingTask::SetPoseState_Implementation(const ASPGameCharacterBase* SPChar, const bool bStart) const
{
	if (SPAnimStateComp == nullptr)
	{
		return;
	}
	MOE_LOG(LogTumblingTask, Log, TEXT("USPTumblingTask::SetPoseState %d"), bStart);

	if (bStart)
	{
		SPAnimStateComp->AnimLayerPoseState = EMoeAnimModule_AnimPoseState::Moe_Action_Body;
		SPAnimStateComp->AnimActionState = m_TumblingStateType;
		SPAnimStateComp->TumblingStarPosition = 0;
	}
	else
	{
		SPAnimStateComp->AnimLayerPoseState = EMoeAnimModule_AnimPoseState::Moe_Locomotion;
		SPAnimStateComp->AnimActionState = EMoeAnimModule_AnimBodyActionState::SP_SkillPlaying;

		bool bIsFalling = false;
		if (SPChar->GetMovementComponent() && SPChar->GetMovementComponent()->IsFalling())
		{
			bIsFalling = true;
		}
		
		if (bIsFalling)
		{
			SPAnimStateComp->LocomotionAnimState = EMoeAnimModule_LocomotionMoveState::Moe_OnFall;
			SPAnimStateComp->SetAnimJumpState(EMoeAnimModule_LocoJumpState::Moe_JumpLoop);
			MOE_LOG(LogTumblingTask, Log, TEXT("USPTumblingTask::SetPoseState on Fall"));
		}

		else
		{
			const bool NeedSetEndingAnimState = SPAnimStateComp->LocomotionAnimState != EMoeAnimModule_LocomotionMoveState::Moe_OnBaseJump;
			if (NeedSetEndingAnimState)
			{
				SPAnimStateComp->LocomotionAnimState = EMoeAnimModule_LocomotionMoveState::Moe_OnGroundMove;
			}
		}
	}
	
	MOE_LOG(LogTumblingTask, Log, TEXT("USPTumblingTask::SetPoseState AnimLayerPoseState:%s"), *UEnum::GetValueAsString(SPAnimStateComp->AnimLayerPoseState));
}

void USPTumblingTask::ResetPerfectDodge_Implementation(const ASPGameCharacterBase* SPChar) const
{
	USPPerfectDodgeComponent* SPPerfectDodgeComp = SPChar->FindComponentByClass<USPPerfectDodgeComponent>();
	if (SPPerfectDodgeComp == nullptr)
	{
		return;
	}
	SPPerfectDodgeComp->ResetTumbling();
}


EAbleAbilityTaskRealm USPTumblingTask::GetTaskRealmBP_Implementation() const { return ATR_ClientAndServer; }

#undef LOCTEXT_NAMESPACE

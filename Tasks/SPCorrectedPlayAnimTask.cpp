#include "Game/SPGame/Skill/Task/SPCorrectedPlayAnimTask.h"

#include "Animation/AnimInstance.h"
#include "Animation/AnimInstanceProxy.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimNode_SPAbilityAnimPlayer.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimSingleNodeInstance.h"
#include "Animation/AnimStateMachineTypes.h"
#include "Animation/AnimNode_StateMachine.h"

#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"

#include "ableSubSystem.h"
#include "ableAbility.h"
#include "ableAbilityComponent.h"

#if !(UE_BUILD_SHIPPING)
#include "ableAbilityUtilities.h"
#endif

DEFINE_LOG_CATEGORY_STATIC(LogSPCorrectedPlayAnimTask, Warning, All);
#define SPABL_GET_DYNAMIC_PROPERTY_VALUE(Context, Property) (Property##Delegate.IsBound() ? Property##Delegate.Execute(Context, Property) : Property)
#define LOCTEXT_NAMESPACE "SPAbleAbilityTask"



USPCorrectedPlayAnimTask::USPCorrectedPlayAnimTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


FString USPCorrectedPlayAnimTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPCorrectedPlayAnimTask");
}

bool USPCorrectedPlayAnimTask::IsDone(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	return IsDoneBP(Context.Get());
}

bool USPCorrectedPlayAnimTask::IsDoneBP_Implementation(const UAbleAbilityContext* Context) const
{
	float PlayRate = m_PlayRate;
	const float EndTime = GetEndTime();
	const float CurrentTime = Context->GetCurrentTime();

	if (PlayRate > SMALL_NUMBER)
	{
		const float StartTime = GetStartTime();
		return CurrentTime > StartTime + (EndTime - StartTime) / PlayRate;
	}
	else
	{
		return CurrentTime > EndTime;
	}
}

/*
* 还原UAblePlayAnimationTask的设计思想
*/
void USPCorrectedPlayAnimTask::OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const
{
	if (!Context || !Context->GetAbility())
	{
		UE_LOG(LogSPCorrectedPlayAnimTask, Warning, TEXT("Empty context in task [%s]"), *GetNameSafe(this));
		return;
	}

	UAbleAbilityTask::OnTaskStart(Context);
	const TSoftObjectPtr<UAnimationAsset> AnimationAssetSoftObjPtr = SPABL_GET_DYNAMIC_PROPERTY_VALUE(Context, m_AnimationAsset);
	const UAnimationAsset* AnimationAsset = AnimationAssetSoftObjPtr.LoadSynchronous();
	if (!IsValid(AnimationAsset))
	{
		UE_LOG(LogSPCorrectedPlayAnimTask, Warning, TEXT("No Animation set for PlayAnimationTask in Ability [%s]"), *Context->GetAbility()->GetAbilityName());
		return;
	}

	TArray<TWeakObjectPtr<AActor>> TargetArray;
	GetActorsForTask(Context, TargetArray);

	UAblePlayAnimationTaskScratchPad* ScratchPad = CastChecked<UAblePlayAnimationTaskScratchPad>(Context->GetScratchPadForTask(this));
	ScratchPad->AbilityComponents.Empty();
	ScratchPad->SingleNodeSkeletalComponents.Empty();
	ScratchPad->CachedVisibilityBasedAnimTickOptionMap.Empty();

	float BasePlayRate = SPABL_GET_DYNAMIC_PROPERTY_VALUE(Context, m_PlayRate);
	float PlayRate = BasePlayRate * (m_ScaleWithAbilityPlayRate ? Context->GetAbility()->GetPlayRate(Context) : 1.0f);
	FName MontageSection = SPABL_GET_DYNAMIC_PROPERTY_VALUE(Context, m_AnimationMontageSection);

	for (TWeakObjectPtr<AActor>& Target : TargetArray)
	{
		if (Target.IsValid())
		{
			if (USkeletalMeshComponent* PreferredComponent = Context->GetAbility()->GetSkeletalMeshComponentForActor(Context, Target.Get(), m_EventName))
			{
				PlayAnimation(Context, AnimationAsset, MontageSection, *Target.Get(), *ScratchPad, *PreferredComponent, PlayRate);
			}
			else
			{
				TInlineComponentArray<USkeletalMeshComponent*> InSkeletalComponents(Target.Get());

				for (UActorComponent* SkeletalComponent : InSkeletalComponents)
				{
					PlayAnimation(Context, AnimationAsset, MontageSection, *Target.Get(), *ScratchPad, *Cast<USkeletalMeshComponent>(SkeletalComponent), PlayRate);
				}
			}

			if (m_OverrideVisibilityBasedAnimTick)
			{
				// Cached origin VisibilityBasesAnimTickOption for restore when animation ended.
				if (USkeletalMeshComponent* TargetMesh = Target.Get()->FindComponentByClass<USkeletalMeshComponent>())
				{
					ScratchPad->CachedVisibilityBasedAnimTickOptionMap.Add(TargetMesh, TargetMesh->VisibilityBasedAnimTickOption);
					TargetMesh->VisibilityBasedAnimTickOption = m_VisibilityBasedAnimTick;
				}
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE

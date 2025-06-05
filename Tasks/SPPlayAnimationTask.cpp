// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SPGame/Skill/Task/SPPlayAnimationTask.h"

#include "Game/SPGame/Character/Animation/SPGameCharacterAnimInstance.h"

FString USPPlayAnimationTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPPlayAnimationTask");
}

UAnimMontage* USPPlayAnimationTask::PlayMontageBySequence(UAnimInstance* Instance, UAnimSequenceBase* Asset,
	FName SlotNodeName, float BlendInTime, float BlendOutTime, float InPlayRate, int32 LoopCount,
	float BlendOutTriggerTime, float InTimeToStartMontageAt, bool bStopAllMontages, float Weight) const
{
	if (Instance)
	{
		if (USPGameCharacterAnimInstance* AnimInstance = Cast<USPGameCharacterAnimInstance>(Instance))
		{
			return AnimInstance->PlayMontageBySequence(Asset, SlotNodeName, BlendInTime, BlendOutTime, InPlayRate, LoopCount,
							  BlendOutTriggerTime, InTimeToStartMontageAt, bStopAllMontages, Weight);
		}
		else
		{
			return Super::PlayMontageBySequence(Instance, Asset, SlotNodeName, BlendInTime, BlendOutTime, InPlayRate, LoopCount,
							  BlendOutTriggerTime,
							  InTimeToStartMontageAt, bStopAllMontages, Weight);
		}
	}
	return nullptr;
}

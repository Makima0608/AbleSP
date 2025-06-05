// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SPGame/Skill/Task/SPPVPPlayParticleEffectTask.h"

#include "ableAbility.h"
#include "AkGameplayStatics.h"
#include "WwiseItemType.h"
#include "Game/SPGame/Utils/SPGameLibrary.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

FString USPPVPPlayParticleEffectTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPPVPPlayParticleEffectTask");
}

USPPVPPlayParticleEffectTask::USPPVPPlayParticleEffectTask(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
}

USPPVPPlayParticleEffectTask::~USPPVPPlayParticleEffectTask()
{
}

void USPPVPPlayParticleEffectTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	OnTaskStartBP_Override(Context.Get());
	Super::OnTaskStart(Context);
}

void USPPVPPlayParticleEffectTask::OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	float deltaTime) const
{
	Super::OnTaskTick(Context, deltaTime);
	OnTaskTickBP_Override(Context.Get(), deltaTime);
}

void USPPVPPlayParticleEffectTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	const EAbleAbilityTaskResult result) const
{
	OnTaskEndBP_Override(Context.Get(), result);
	Super::OnTaskEnd(Context, result);
}

void USPPVPPlayParticleEffectTask::OnTaskEndBP_Override_Implementation(const UAbleAbilityContext* Context,
                                                                       const EAbleAbilityTaskResult result) const
{
	
}

void USPPVPPlayParticleEffectTask::OnTaskTickBP_Override_Implementation(const UAbleAbilityContext* Context,
                                                                        float deltaTime) const
{
}

void USPPVPPlayParticleEffectTask::OnTaskStartBP_Override_Implementation(const UAbleAbilityContext* Context) const
{
	SetParticleEffectPos_Lua(Context);
}

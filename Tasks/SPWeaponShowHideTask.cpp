// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SPGame/Skill/Task/SPWeaponShowHideTask.h"

#include "ableSubSystem.h"
#include "Game/SPGame/Gameplay/SPActorInterface.h"
#include "Game/SPGame/Monster/SPGameCharInventoryComponent.h"
#include "Game/SPGame/Utils/SPGameLibrary.h"
#include "Game/SPGame/Weapon/SPWeapon.h"
#include "Game/SPGame/Weapon/Component/SPWeaponVisibilityComponent.h"
#include "MoeGameplay/Character/Component/MoeCharInventoryComponent.h"

USPWeaponShowHideTaskScratchPad::USPWeaponShowHideTaskScratchPad(): OriginWeaponVisibility(false)
{
}

USPWeaponShowHideTaskScratchPad::~USPWeaponShowHideTaskScratchPad()
{
}

USPWeaponShowHideTask::USPWeaponShowHideTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	HideWeapon(false),
	m_Target(EAbleAbilityTargetType::ATT_Self)
{
	
}

FString USPWeaponShowHideTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPWeaponShowHideTask");
}

void USPWeaponShowHideTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);
	OnTaskStartBP(Context.Get());
}

void USPWeaponShowHideTask::OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const
{
	USPWeaponShowHideTaskScratchPad* ScratchPad = Cast<USPWeaponShowHideTaskScratchPad>(Context->GetScratchPadForTask(this));
	if(ScratchPad == nullptr)
		return;

	ISPActorInterface* Actor = Cast<ISPActorInterface>(GetSingleActorFromTargetType(Context, m_Target));
	if (!Actor) return;

	USPGameCharInventoryComponent* CharInventoryComponent = Cast<USPGameCharInventoryComponent>(Actor->SPGetInventoryComponent());
	if(CharInventoryComponent == nullptr)
		return;

	const ASPWeapon* CurEquipWeapon =  Cast<ASPWeapon>(CharInventoryComponent->GetCrtEquipWeapon());
	if(CurEquipWeapon == nullptr)
		return;

	ScratchPad->OriginWeaponVisibility = CurEquipWeapon->GetWeaponMeshVisibility();
	CharInventoryComponent->SPSetWeaponHideInGame(HideWeapon);
}

EAbleAbilityTaskRealm USPWeaponShowHideTask::GetTaskRealmBP_Implementation() const
{
	return EAbleAbilityTaskRealm::ATR_Client;
}

void USPWeaponShowHideTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	const EAbleAbilityTaskResult result) const
{
	Super::OnTaskEnd(Context, result);
	OnTaskEndBP(Context.Get(), result);
}

void USPWeaponShowHideTask::OnTaskEndBP_Implementation(const UAbleAbilityContext* Context,
	                 const EAbleAbilityTaskResult Result) const
{
	USPWeaponShowHideTaskScratchPad* ScratchPad = Cast<USPWeaponShowHideTaskScratchPad>(Context->GetScratchPadForTask(this));
	if(ScratchPad == nullptr)
		return;

	ISPActorInterface* Actor = Cast<ISPActorInterface>(GetSingleActorFromTargetType(Context, m_Target));
	if (!Actor) return;

	USPGameCharInventoryComponent* CharInventoryComponent = Cast<USPGameCharInventoryComponent>(Actor->SPGetInventoryComponent());
	if(CharInventoryComponent == nullptr)
		return;

	if (!bIsSingleFrame && bResetAtEnd)
	{
		USPWeaponVisibilityComponent* WeaponVisibilityComponent = Cast<USPWeaponVisibilityComponent>(Actor->SPGetWeaponVisibilityComponent());
		if (!WeaponVisibilityComponent)
		{
			return;
		}

		const bool WeaponVisibility = WeaponVisibilityComponent->CanShowWeapon();
		CharInventoryComponent->SPSetWeaponHideInGame(!WeaponVisibility);
	}
}

TStatId USPWeaponShowHideTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPWeaponShowHideTask, STATGROUP_USPAbility);
}

UAbleAbilityTaskScratchPad* USPWeaponShowHideTask::CreateScratchPad(
	const TWeakObjectPtr<UAbleAbilityContext>& Context) const
{
	if (UAbleAbilityUtilitySubsystem* Subsystem = Context->GetUtilitySubsystem())
	{
		static TSubclassOf<UAbleAbilityTaskScratchPad> ScratchPadClass = USPWeaponShowHideTaskScratchPad::StaticClass();
		return Subsystem->FindOrConstructTaskScratchPad(ScratchPadClass);
	}
	return NewObject<USPWeaponShowHideTaskScratchPad>(Context.Get());
}

bool USPWeaponShowHideTask::IsSingleFrameBP_Implementation() const { return bIsSingleFrame; }

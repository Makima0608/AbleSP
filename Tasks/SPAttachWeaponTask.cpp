// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SPGame/Skill/Task/SPAttachWeaponTask.h"

#include "ableSubSystem.h"
#include "MoeFeatureSPLog.h"
#include "MoeGameLog.h"
#include "Game/SPGame/Character/SPGameCharacterBase.h"
#include "Game/SPGame/Monster/SPGameCharInventoryComponent.h"
#include "Game/SPGame/Skill/Core/SPAbilityFunctionLibrary.h"
#include "Game/SPGame/Utils/SPGameLibrary.h"
#include "MoeGameplay/Character/MoeGameCharacter.h"

USAttachWeaponTaskScratchPad::USAttachWeaponTaskScratchPad()
{
}

USAttachWeaponTaskScratchPad::~USAttachWeaponTaskScratchPad()
{
}

USPAttachWeaponTask::USPAttachWeaponTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), m_Target(EAbleAbilityTargetType::ATT_Self), m_PreviewWeapon(nullptr), bHideWeaponMesh(false)
{
}

USPAttachWeaponTask::~USPAttachWeaponTask()
{
}

FString USPAttachWeaponTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPAttachWeaponTask");
}

void USPAttachWeaponTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);
	OnTaskStartBP(Context.Get());
}

void USPAttachWeaponTask::OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const
{
	USAttachWeaponTaskScratchPad* ScratchPad = Cast<USAttachWeaponTaskScratchPad>(
		Context->GetScratchPadForTask(this));
	if (!ScratchPad) return;

	AActor* AttachActor = GetSingleActorFromTargetType(Context, m_Target);
	if (!IsValid(AttachActor)) return;

	USkeletalMeshComponent* SkeletonMeshComponent = Cast<USkeletalMeshComponent>(
		AttachActor->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	if (!IsValid(SkeletonMeshComponent)) return;

	AActor* WeaponActor = CreateWeaponToAttach(AttachActor, SkeletonMeshComponent);
	if (!IsValid(WeaponActor))
	{
		if (m_PreviewWeapon)
		{
			WeaponActor = CreatDefaultToAttach(AttachActor, SkeletonMeshComponent);
		}

		// check again after default
		if (!IsValid(WeaponActor))
		{
			MOE_SP_ABILITY_ERROR(
				TEXT("[SPAbility] USPAttachWeaponTask attach failed, decoration component null after load !"));
			return;
		}
	}

	WeaponActor->AttachToComponent(SkeletonMeshComponent,
	                               FAttachmentTransformRules::KeepRelativeTransform,
	                               m_Socket);
	WeaponActor->SetActorRelativeTransform(m_RelativeTransform);
	ScratchPad->CacheWeapon = WeaponActor;

	// hide weapon
	USPGameCharInventoryComponent* InventoryComponent = Cast<USPGameCharInventoryComponent>(
		AttachActor->GetComponentByClass(USPGameCharInventoryComponent::StaticClass()));
	if (IsValid(InventoryComponent))
	{
		if (bHideWeaponMesh)
		{
			InventoryComponent->SPSetWeaponMeshHideInGame(true);
		}
		else
		{
			InventoryComponent->SPSetWeaponHideInGame(true);
		}
	}
}

void USPAttachWeaponTask::OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
                                     float deltaTime) const
{
	Super::OnTaskTick(Context, deltaTime);
	OnTaskTickBP(Context.Get(), deltaTime);
}

void USPAttachWeaponTask::OnTaskTickBP_Implementation(const UAbleAbilityContext* Context, float deltaTime) const
{

#if WITH_EDITOR
	const USAttachWeaponTaskScratchPad* ScratchPad = Cast<USAttachWeaponTaskScratchPad>(
		Context->GetScratchPadForTask(this));
	if (!ScratchPad) return;

	if (IsValid(ScratchPad->CacheWeapon))
		ScratchPad->CacheWeapon->SetActorRelativeTransform(m_RelativeTransform);

#endif
}

void USPAttachWeaponTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
                                    const EAbleAbilityTaskResult result) const
{
	Super::OnTaskEnd(Context, result);
	OnTaskEndBP(Context.Get(), result);
}

void USPAttachWeaponTask::OnTaskEndBP_Implementation(const UAbleAbilityContext* Context,
                                                     const EAbleAbilityTaskResult result) const
{
	USAttachWeaponTaskScratchPad* ScratchPad = Cast<USAttachWeaponTaskScratchPad>(
		Context->GetScratchPadForTask(this));
	if (!ScratchPad) return;

	if (IsValid(ScratchPad->CacheWeapon))
	{
		ScratchPad->CacheWeapon->K2_DestroyActor();
		ScratchPad->CacheWeapon = nullptr;
	}
	else
	{
		MOE_SP_ABILITY_WARN(
			TEXT(
				"[SPAbility] USPAttachWeaponTask::OnTaskEnd Warning ! Destroy Decoration Mesh failed because component invalid !"
			))
	}

	// show weapon
	const AActor* AttachActor = GetSingleActorFromTargetType(Context, m_Target);
	if (!IsValid(AttachActor)) return;

	USPGameCharInventoryComponent* InventoryComponent = Cast<USPGameCharInventoryComponent>(
		AttachActor->GetComponentByClass(USPGameCharInventoryComponent::StaticClass()));
	if (IsValid(InventoryComponent))
	{
		if (bHideWeaponMesh)
		{
			InventoryComponent->SPSetWeaponMeshHideInGame(false);
		}
		else
		{
			InventoryComponent->SPSetWeaponHideInGame(false);
		}
	}
}

AActor* USPAttachWeaponTask::CreateWeaponToAttach(AActor* AttachActor,
                                                  const USkeletalMeshComponent* SkeletonMeshComponent) const
{
	const FTransform RelativeTrans = FTransform(SkeletonMeshComponent->GetRelativeTransform());
	AActor* DecorationMeshComponent = nullptr;

	ASPGameCharacterBase* Character = Cast<ASPGameCharacterBase>(AttachActor);
	if (!Character)
	{
		MOE_SP_ABILITY_ERROR(
			TEXT("[SPAbility] USPAttachWeaponTask attach failed, Target Actor not a AMoeGameCharacter !"));
		return nullptr;
	}

	// const AWeapon* CurrentWeapon = Character->GetCrtEquipWeapon();
	const int32 CurrentToolId = Character->GetCurUsingToolID();
	// if (!IsValid(CurrentTool))
	if (CurrentToolId <= 0)
	{
		MOE_SP_ABILITY_WARN(
			TEXT("[SPAbility] USPAttachWeaponTask attach failed, Target Actor not equip Tool !"));
		return nullptr;
	}

	FSPWeaponConfig Config;
	const bool bRet = USPGameLibrary::GetWeaponConfig(CurrentToolId, Config, Character->GetWorld(), Character);
	RETURN_NULLPTR_IF(!bRet,
	                  TEXT("[SPAbility] USPAttachWeaponTask attach failed, cannot find weapon config with id %d"),
	                  CurrentToolId);

	UClass* Class = USPAbilityFunctionLibrary::LoadClassWithFullPath(Config.resPath, Character->GetWorld());
	AActor* WeaponActor = Character->GetWorld()->SpawnActor(Class, &RelativeTrans);
	return WeaponActor;
}

AActor* USPAttachWeaponTask::CreatDefaultToAttach(AActor* AttachActor,
												  const USkeletalMeshComponent* SkeletonMeshComponent) const
{
	const FTransform RelativeTrans = FTransform(SkeletonMeshComponent->GetRelativeTransform());
	ASPGameCharacterBase* Character = Cast<ASPGameCharacterBase>(AttachActor);
	if (!Character)
	{
		MOE_SP_ABILITY_ERROR(
			TEXT("[SPAbility] USPAttachWeaponTask attach failed, Target Actor not a AMoeGameCharacter !"));
		return nullptr;
	}

	AActor* WeaponActor = Character->GetWorld()->SpawnActor(m_PreviewWeapon, &RelativeTrans);
	return WeaponActor;
}

UAbleAbilityTaskScratchPad* USPAttachWeaponTask::CreateScratchPad(
	const TWeakObjectPtr<UAbleAbilityContext>& Context) const
{
	if (UAbleAbilityUtilitySubsystem* Subsystem = Context->GetUtilitySubsystem())
	{
		static TSubclassOf<UAbleAbilityTaskScratchPad> ScratchPadClass =
			USAttachWeaponTaskScratchPad::StaticClass();
		return Subsystem->FindOrConstructTaskScratchPad(ScratchPadClass);
	}

	return NewObject<USAttachWeaponTaskScratchPad>(Context.Get());
}

TStatId USPAttachWeaponTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPAttachWeaponTask, STATGROUP_USPAbility);
}

bool USPAttachWeaponTask::IsSingleFrameBP_Implementation() const { return false; }

#undef LOCTEXT_NAMESPACE

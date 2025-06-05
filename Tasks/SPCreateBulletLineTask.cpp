// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SPGame/Skill/Task/SPCreateBulletLineTask.h"

#include "ableSubSystem.h"
#include "MoeFeatureSPLog.h"
#include "Game/SPGame/Projectile/SPProjectileBase.h"
#include "Game/SPGame/Utils/SPGameLibrary.h"
#include "Game/SPGame/Weapon/SPWeapon.h"
#include "Game/SPGame/Weapon/WeaponFireComponent/SPWeaponFireComponent_PetRange.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

USPCreateBulletLineTaskScratchPad::USPCreateBulletLineTaskScratchPad(): OutActorsNum(0)
{
}

USPCreateBulletLineTaskScratchPad::~USPCreateBulletLineTaskScratchPad()
{
}

USPCreateBulletLineTask::USPCreateBulletLineTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	OwnerTargetType(EAbleAbilityTargetType::ATT_Self)
{
}

USPCreateBulletLineTask::~USPCreateBulletLineTask()
{
}

void USPCreateBulletLineTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);
	OnSingleShoot(Context);
}

void USPCreateBulletLineTask::OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	float DeltaTime) const
{
	Super::OnTaskTick(Context, DeltaTime);

	if (!IsKeepFiring())
	{
		return;
	}

	const float CurrentTime = UGameplayStatics::GetTimeSeconds(GetWorld());
	if (CurrentTime - LastShootTime > BulletCDTime)
	{
		OnSingleShoot(Context);
	}
}

void USPCreateBulletLineTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
                                        const EAbleAbilityTaskResult result) const
{
	Super::OnTaskEnd(Context, result);
	ASPGameCharacterBase* Player = GetPlayer(Context);
	if (Player)
	{
		Player->RemoveIgnoreActor(Context->GetSelfActor());
	}
}

UAbleAbilityTaskScratchPad* USPCreateBulletLineTask::CreateScratchPad(
	const TWeakObjectPtr<UAbleAbilityContext>& Context) const
{
	if (UAbleAbilityUtilitySubsystem* Subsystem = Context->GetUtilitySubsystem())
	{
		static TSubclassOf<UAbleAbilityTaskScratchPad> ScratchPadClass =
			USPCreateBulletLineTaskScratchPad::StaticClass();
		return Subsystem->FindOrConstructTaskScratchPad(ScratchPadClass);
	}
	return NewObject<USPCreateBulletLineTaskScratchPad>(Context.Get());
}

TStatId USPCreateBulletLineTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPCreateBulletLineTask, STATGROUP_USPAbility);
}

void USPCreateBulletLineTask::SetSpawnParams(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	FActorSpawnParameters& SpawnParams) const
{
	SpawnParams.Owner = GetSingleActorFromTargetType(Context, OwnerTargetType);
	SpawnParams.Instigator = Cast<APawn>(Context->GetInstigator());
	SpawnParams.ObjectFlags = EObjectFlags::RF_Transient;
}

void USPCreateBulletLineTask::AddOutActors(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
                                           TArray<TWeakObjectPtr<AActor>>& OutActors) const
{
	if (OutActors.Num() == 0)
	{
		OutActors.Add(Context->GetSelfActor());
	}
}

void USPCreateBulletLineTask::DestroyAtEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
                                           USPCreateBulletLineTaskScratchPad*& ScratchPad) const
{
	ScratchPad = Cast<USPCreateBulletLineTaskScratchPad>(Context->GetScratchPadForTask(this));
	if(ScratchPad)
	{
		ScratchPad->SpawnedActors.Empty();
		ScratchPad->SpawnInfos.Empty();
	}
}

FVector USPCreateBulletLineTask::GetProjectileDirection(AActor* Owner) const
{
	FRotator OwnerRotation = Owner->GetActorRotation();
	ASPGameMonsterBase* Monster = Cast<ASPGameMonsterBase>(Owner);
	if (IsValid(Monster))
	{
		if (Monster->bIsFourFootMonster)
		{
			OwnerRotation = Monster->GetRotationVerticalGround();
			return OwnerRotation.Vector();
		}
	}
	ASPGameCharacterBase* Character = Cast<ASPGameCharacterBase>(Owner);
	if (IsValid(Character))
	{
		OwnerRotation = Character->GetMesh()->GetComponentRotation();
		OwnerRotation.Yaw += 90;
		return OwnerRotation.Vector();
	}
	return OwnerRotation.Vector();
}

void USPCreateBulletLineTask::CreateBulletLine(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	FVector StartPos, FVector ShootDirection) const
{
	ASPGameCharacterBase* Player = GetPlayer(Context);
	if (Player)
	{
		if (const ASPWeapon* SPWeapon = Cast<ASPWeapon>(Player->GetCrtEquipWeapon()))
		{
			if (SPWeapon->IsSelfWeapon())
			{
				if (USPWeaponFireComponent_PetRange* FireComponent = Cast<USPWeaponFireComponent_PetRange>(SPWeapon->FireComponent))
				{
					FireComponent->CreateBulletLineTask(StartPos, UKismetMathLibrary::Conv_VectorToQuaternion(UKismetMathLibrary::Normal(ShootDirection)));
				}
			}
		}
	}
}

void USPCreateBulletLineTask::SpawnProjectiles(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	USPCreateBulletLineTaskScratchPad* ScratchPad = Cast<USPCreateBulletLineTaskScratchPad>(Context->GetScratchPadForTask(this));
	if(ScratchPad == nullptr)
		return;

	int SpawnNums = AmountToSpawn;
	for (int32 i = 0; i < ScratchPad->OutActorsNum; ++i)
	{
		FShootProjectileTaskSpawnInfo SpawnInfo = ScratchPad->SpawnInfos[i];
		for (int32 SpawnIndex = 0; SpawnIndex < SpawnNums; ++SpawnIndex)
		{
			FTransform SpawnTransform;
			SpawnTransform = SpawnInfo.GetSpawnTransformByIndex(SpawnIndex);

			auto SelfActor = Context->GetSelfActor();
			FVector StartPosition = SpawnTransform.GetLocation();
			FVector ShootDirection = USPGameLibrary::GetCrossHairDirection(SelfActor, SpawnTransform.GetLocation(),SelfActor, ESPTraceType::Weapon,GetProjectileDirection(SelfActor));
			CreateBulletLine(Context, StartPosition, ShootDirection);
		}
	}
}

void USPCreateBulletLineTask::ApplyShotSpread(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	AActor* Owner = Context->GetSelfActor();
	ASPGameCharacterBase* Character = Cast<ASPGameCharacterBase>(Owner);
	if (!IsValid(Character))
	{
		const ASPGameMonsterBase* Monster = Cast<ASPGameMonsterBase>(Owner);
		if (IsValid(Monster))
		{
			Character = Cast<ASPGameCharacterBase>(Monster->FindRelatedPlayerPawn());
		}
	}
	if (IsValid(Character))
	{
		if (ASPWeapon* SPWeapon = Cast<ASPWeapon>(Character->GetCrtEquipWeapon()))
		{
			if (USPWeaponFireComponent_PetRange* FireComponent = Cast<USPWeaponFireComponent_PetRange>(SPWeapon->FireComponent))
			{
				FireComponent->ApplyShotSpread();
			}
		}
	}
}

ASPGameCharacterBase* USPCreateBulletLineTask::GetPlayer(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	AActor* Owner = Context->GetSelfActor();
	ASPGameCharacterBase* Character = Cast<ASPGameCharacterBase>(Owner);
	if (!IsValid(Character))
	{
		const ASPGameMonsterBase* Monster = Cast<ASPGameMonsterBase>(Owner);
		if (IsValid(Monster))
		{
			Character = Cast<ASPGameCharacterBase>(Monster->FindRelatedPlayerPawn());
		}
	}
	return Character;
}

void USPCreateBulletLineTask::InitSpawnTargetLocationForTargetActor(
	const TWeakObjectPtr<const UAbleAbilityContext>& Context, int32 TargetIndex, TArray<FTransform>& OutTransform) const
{
	const AActor* AttachActor = GetSingleActorFromTargetType(Context, OwnerTargetType);
	if (!IsValid(AttachActor))
	{
		MOE_SP_ABILITY_WARN(
			TEXT("[SPAbility] USPCreateBulletLineTask::InitSpawnTargetLocationForTargetActor failed, can not find owner target!"));
		return;
	}

	const USkeletalMeshComponent* SkeletonMeshComponent = Cast<USkeletalMeshComponent>(
		AttachActor->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	if (!IsValid(SkeletonMeshComponent))
	{
		MOE_SP_ABILITY_WARN(
			TEXT("[SPAbility] USPCreateBulletLineTask::InitSpawnTargetLocationForTargetActor find target actor skeletalmesh failed!"));
		return;
	}

	TArray<USceneComponent*> Children = SkeletonMeshComponent->GetAttachChildren();
	for (auto Location : SpawnSocketLocationList)
	{
		FTransform SocketTransform;
		for (const auto Child : Children)
		{
			if (Location.Socket == Child->GetAttachSocketName())
			{
				SocketTransform = Child->GetSocketTransform(Location.MuzzleSocket);
				break;
			}
		}
		OutTransform.Add(SocketTransform);
	}
}

bool USPCreateBulletLineTask::IsKeepFiring() const
{
	return BulletCDTime > 0;
}

void USPCreateBulletLineTask::OnSingleShoot(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	USPCreateBulletLineTaskScratchPad* ScratchPad = nullptr;
	DestroyAtEnd(Context, ScratchPad);

	ASPGameCharacterBase* Player = GetPlayer(Context);
	if (Player)
	{
		Player->AddIgnoreActor(Context->GetSelfActor());
	}

	TArray<TWeakObjectPtr<AActor>> OutActors;
	GetActorsForTask(Context, OutActors);
	AddOutActors(Context, OutActors);

	if(ScratchPad)
	{
		ScratchPad->OutActorsNum = OutActors.Num();
	}

	for (int32 i = 0; i < OutActors.Num(); ++i)
	{
		FActorSpawnParameters SpawnParams;
		SetSpawnParams(Context, SpawnParams);

		TArray<FTransform> SpawnTransformList;
		InitSpawnTargetLocationForTargetActor(Context, i, SpawnTransformList);

		if(ScratchPad)
		{
			ScratchPad->SpawnInfos.Add(FShootProjectileTaskSpawnInfo(nullptr, SpawnTransformList, SpawnParams));
		}
	}

	LastShootTime = UGameplayStatics::GetTimeSeconds(GetWorld());

	if (bEnableShotSpread)
	{
		ApplyShotSpread(Context);
	}

	SpawnProjectiles(Context);
}

#undef LOCTEXT_NAMESPACE
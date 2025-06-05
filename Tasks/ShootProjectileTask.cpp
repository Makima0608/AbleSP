#include "Game/SPGame/Skill/Task/ShootProjectileTask.h"

#include "ableAbility.h"
#include "ableSubSystem.h"
#include "MoeFeatureSPLog.h"
#include "Engine/World.h"
#include "Game/SPGame/Capture/SPCaptureComponent.h"
#include "Game/SPGame/Capture/Projectile/SPProjectile_Capture.h"
#include "Game/SPGame/Capture/Projectile/SPProjectile_Unleash.h"
#include "Game/SPGame/Character/SPGameMonsterBase.h"
#include "Game/SPGame/Projectile/SPProjectileBase.h"
#include "Game/SPGame/Skill/Core/SPAbilityFunctionLibrary.h"
#include "Game/SPGame/SubSystem/SPGameplaySubsystem.h"
#include "Game/SPGame/Utils/SPGameLibrary.h"
#include "Game/SPGame/Weapon/SPWeapon.h"
#include "Game/SPGame/Weapon/WeaponFireComponent/SPWeaponFireComponent_PetRange.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"
#define REASON_PROJECTILE_COST 3085
//DECLARE_STATS_GROUP(TEXT("SPSkillAbility"), STATGROUP_SPSkillAbility, STATCAT_Advanced);

UShootProjectileTaskScratchPad::UShootProjectileTaskScratchPad()
{
}

UShootProjectileTaskScratchPad::~UShootProjectileTaskScratchPad()
{
}

UShootProjectileTask::UShootProjectileTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	m_ActorClass(),
	m_bIsForCapture(false),
	m_CaptureFunctionType(ECaptureFunctionType::Capture),
	m_bCostItem(false),
	m_CostItemId(-1),
	m_AmountToSpawn(1),
	m_AimingTargetSocket(NAME_None),
	m_SoleTarget(true),
	m_SpawnWhenTargetEmpty(true),
	m_InnoreLocationSourceActor(true),
	m_SpawnCollision(ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn),
	m_SetOwner(true),
	m_OwnerTargetType(EAbleAbilityTargetType::ATT_Self),
	m_MarkAsTransient(true),
	m_DestroyAtEnd(false),
	m_FireEvent(false),
	m_Name(NAME_None),
	m_TaskRealm(EAbleAbilityTaskRealm::ATR_Server),
	m_bLoopShoot(false),
	m_IntervalTime(0.0),
	m_TotalShootTimes(1)
{
}

UShootProjectileTask::~UShootProjectileTask()
{
}


void UShootProjectileTask::SetSpawnParams(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
                                          FActorSpawnParameters& SpawnParams) const
{
	// always set owner
	// if (m_SetOwner)
	{
		SpawnParams.Owner = GetSingleActorFromTargetType(Context, m_OwnerTargetType);
		SpawnParams.Instigator = Cast<APawn>(Context->GetInstigator());
	}

	SpawnParams.SpawnCollisionHandlingOverride = m_SpawnCollision;

	if (m_MarkAsTransient)
	{
		SpawnParams.ObjectFlags = EObjectFlags::RF_Transient; // We don't want to save anything on this object.
	}
}

void UShootProjectileTask::JudgeShootType(ASPProjectileBase*& SpawnedActor) const
{
	if (m_ShootType == ESPShootType::Auto)
	{
		SpawnedActor->ShootProjectile();
	}
	else if (m_ShootType == ESPShootType::Timed)
	{
		if (m_ShootDelay >= 0.0f)
		{
			SpawnedActor->SetShootTimer(true, m_ShootDelay);
		}
		else
		{
			SpawnedActor->SetShootTimer(false);
		}
	}
}

void UShootProjectileTask::AddOutActors(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
                                        TArray<TWeakObjectPtr<AActor>>& OutActors) const
{
	if (OutActors.Num() == 0)
	{
		OutActors.Add(Context->GetSelfActor());
	}
}

void UShootProjectileTask::DestroyAtEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
                                        UShootProjectileTaskScratchPad*& ScratchPad) const
{
	ScratchPad = Cast<UShootProjectileTaskScratchPad>(Context->GetScratchPadForTask(this));
	if(ScratchPad)
	{
		ScratchPad->SpawnedActors.Empty();
		ScratchPad->SpawnInfos.Empty();
	}
}

FString UShootProjectileTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.ShootProjectileTask");
}

void UShootProjectileTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	check(Context.IsValid());

	Super::OnTaskStart(Context);
	OnCalcMaxDamageCount(Context.Get());
	UE_LOG(LogSPAbility,Log, TEXT("ShootProjectileTask::OnTaskStart"))
	OnTaskStartBP(Context.Get());
}

void UShootProjectileTask::OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context, float deltaTime) const
{
	Super::OnTaskTick(Context, deltaTime);
	OnTaskTickBP(Context.Get(), deltaTime);
}

void UShootProjectileTask::OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const
{

	TSubclassOf<AActor> ActorClass = m_ActorClass.LoadSynchronous();

	if(m_bIsForCapture)
	{
		auto CaptureOverrideClass = CaptureOverride(Context);
		if(UKismetSystemLibrary::IsValidClass(CaptureOverrideClass))
		{
			ActorClass = CaptureOverrideClass;
		}
	}

	// 初始化ScratchPad
	UShootProjectileTaskScratchPad* ScratchPad = nullptr;
	DestroyAtEnd(Context, ScratchPad);

	if (!ActorClass.GetDefaultObject())
	{
		MOE_SP_ABILITY_ERROR(TEXT("[SPSkill] ShootProjectileTask for Ability [%s] does not have a class specified."),
		              *(Context->GetAbility()->GetName()));
		return;
	}
	
	TArray<TWeakObjectPtr<AActor>> OutActors;
	GetActorsForTask(Context, OutActors);
	AddOutActors(Context, OutActors);

	// 存入scratch pad
	if(ScratchPad)
	{
		ScratchPad->OutActorsNum = OutActors.Num();
		ScratchPad->CurrentShootTimes = 0;
		ScratchPad->CurTimeStamp = Context->GetCurrentTime();
	}

	FAbleAbilityTargetTypeLocation SpawnTargetLocation = m_SpawnLocation;
	FTransform SpawnTransform;
	SpawnTargetLocation.GetTransform(*Context, SpawnTransform);

	for (int32 i = 0; i < OutActors.Num(); ++i)
	{
		FActorSpawnParameters SpawnParams;
		SetSpawnParams(Context, SpawnParams);

		if (IsSeparateLocation())
		{
			TArray<FTransform> SpawnTransformList;
			InitSpawnTargetLocationForTargetActor(*Context, i, SpawnTransformList);

			if(ScratchPad)
			{
				ScratchPad->SpawnInfos.Add(FShootProjectileTaskSpawnInfo(ActorClass, SpawnTransformList, SpawnParams));
			}
		}
		else
		{
			if (SpawnTargetLocation.GetSourceTargetType() == EAbleAbilityTargetType::ATT_TargetActor)
			{
				SpawnTargetLocation.GetTargetTransform(*Context, i, SpawnTransform);
			}

#if !(UE_BUILD_SHIPPING)
			if (IsVerbose())
			{
				PrintVerbose(Context, FString::Printf(
								 TEXT("Spawning Actor %s using Transform %s."), *ActorClass->GetName(),
								 *SpawnTransform.ToString()));
			}
#endif

			if(ScratchPad)
			{
				ScratchPad->SpawnInfos.Add(FShootProjectileTaskSpawnInfo(ActorClass,SpawnTransform,SpawnParams));
			}
		}
	}

	if(m_SpawnDelay > 0.)
	{
		// 设置子弹spawn的timer
		FTimerDelegate SpawnTimerDel;
		SpawnTimerDel.BindLambda([this,Context]()
		{
			SpawnProjectiles(Context);
		});
		if(auto World = Context->GetWorld())
			World->GetTimerManager().SetTimer(ScratchPad->SpawnTimer, SpawnTimerDel,m_SpawnDelay,false);
	}
	else
	{
		SpawnProjectiles(Context);
	}
	
}


FVector UShootProjectileTask::GetProjectileDirection(const TWeakObjectPtr<const UAbleAbilityContext>& Context, FVector SpawnTransform) const
{
	AActor* Owner = Context->GetSelfActor();
	if (m_AimTowardTargetLocation)
	{
		FVector TargetLocation;
		if (m_AimTargetLocationType == EAimTargetLocationType::AimTowardSTP)
		{
			TargetLocation = GetProjectileAimLocation_lua(Owner);
		}
		else if (m_AimTargetLocationType == EAimTargetLocationType::AimTowardTargetLocation)
		{
			FTransform TargetTransform;
			m_AimTargetLocation.GetTransform(*Context.Get(), TargetTransform);
			TargetLocation = TargetTransform.GetLocation();
		}
		return (TargetLocation - SpawnTransform).GetSafeNormal();
	}
	else
	{
		if (m_AbsoluteDirection)
		{
			return m_ShootDirection;
		}
		FRotator OwnerRotation = Owner->GetActorRotation();
		ASPGameMonsterBase* Monster = Cast<ASPGameMonsterBase>(Owner);
		if (IsValid(Monster))
		{
			if (Monster->bIsFourFootMonster)
			{
				OwnerRotation = Monster->GetRotationVerticalGround();
				if (m_ShootDirection.Equals(FVector::ZeroVector, 0.0001f))
				{
					return OwnerRotation.Vector();
				}
				else
				{
					return OwnerRotation.RotateVector(m_ShootDirection);
				}
			}
		}
		ASPGameCharacterBase* Character = Cast<ASPGameCharacterBase>(Owner);
		if (IsValid(Character))
		{
			OwnerRotation = Character->GetMesh()->GetComponentRotation();
			OwnerRotation.Yaw += 90;
			if (m_ShootDirection.Equals(FVector::ZeroVector, 0.0001f))
			{
				return OwnerRotation.Vector();
			}
		}
		return OwnerRotation.RotateVector(m_ShootDirection);
	}
}

FVector UShootProjectileTask::GetCrossHairDirection(AActor* Owner) const
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

void UShootProjectileTask::OnTaskTickBP_Implementation(const UAbleAbilityContext* Context, float deltaTime) const
{
	UShootProjectileTaskScratchPad* ScratchPad = Cast<UShootProjectileTaskScratchPad>(Context->GetScratchPadForTask(this));
	if (!ScratchPad || !m_bLoopShoot)
	{
		return;
	}

	LoopSpawnProjectilesByTimeSpanBP(Context);
}

void UShootProjectileTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
                                     const EAbleAbilityTaskResult result) const
{
	Super::OnTaskEnd(Context, result);
	UE_LOG(LogSPAbility,Log, TEXT("ShootProjectileTask::OnTaskEnd"))

	UShootProjectileTaskScratchPad* ScratchPad = Cast<UShootProjectileTaskScratchPad>(Context->GetScratchPadForTask(this));

	if(ScratchPad && m_bLoopShoot)
	{
		LoopSpawnProjectilesByTimeSpanBP(Context.Get());
	}
	OnTaskEndBP(Context.Get(), result);
}

void UShootProjectileTask::OnTaskEndBP_Implementation(const UAbleAbilityContext* Context,
                                                      const EAbleAbilityTaskResult result) const
{

	UShootProjectileTaskScratchPad* ScratchPad = Cast<UShootProjectileTaskScratchPad>(
			Context->GetScratchPadForTask(this));
	if (m_DestroyAtEnd && Context)
	{
		if (ScratchPad && ScratchPad->SpawnedActors.Num())
		{
			for (const auto& SpawnedActor : ScratchPad->SpawnedActors)
			{
#if !(UE_BUILD_SHIPPING)
				if (IsVerbose())
				{
					PrintVerbose(
						Context, FString::Printf(TEXT("Destroying Spawned Actor %s."), *SpawnedActor->GetName()));
				}
#endif
				if (IsValid(SpawnedActor))
				{
					SpawnedActor->TerminateProjectile(ESPProjectileTerminateReason::AbleEnd, SpawnedActor->GetActorLocation());
				}
			}
		}
	}

	if(ScratchPad)
		ScratchPad->SpawnedActors.Empty();

	// 清除生成Timer
	if(ScratchPad && ScratchPad->SpawnTimer.IsValid() && Context)
	{
		if(auto World = Context->GetWorld())
			World->GetTimerManager().ClearTimer(ScratchPad->SpawnTimer);
	}
}

UAbleAbilityTaskScratchPad* UShootProjectileTask::CreateScratchPad(
	const TWeakObjectPtr<UAbleAbilityContext>& Context) const
{
	
	if (UAbleAbilityUtilitySubsystem* Subsystem = Context->GetUtilitySubsystem())
	{
		static TSubclassOf<UAbleAbilityTaskScratchPad> ScratchPadClass =
			UShootProjectileTaskScratchPad::StaticClass();
		return Subsystem->FindOrConstructTaskScratchPad(ScratchPadClass);
	}

	return NewObject<UShootProjectileTaskScratchPad>(Context.Get());
}

TStatId UShootProjectileTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UShootProjectileTask, STATGROUP_USPAbility);
}

void UShootProjectileTask::BindDynamicDelegates(UAbleAbility* Ability)
{
	Super::BindDynamicDelegates(Ability);

	ABL_BIND_DYNAMIC_PROPERTY(Ability, m_ActorClass, TEXT("Projectile Class"));
	ABL_BIND_DYNAMIC_PROPERTY(Ability, m_AmountToSpawn, TEXT("Amount to Spawn"));
	ABL_BIND_DYNAMIC_PROPERTY(Ability, m_SpawnLocation, TEXT("Location"));
	ABL_BIND_DYNAMIC_PROPERTY(Ability, m_DamageIndex, TEXT("Damage Index"));
	ABL_BIND_DYNAMIC_PROPERTY(Ability, m_DamageId, TEXT("Damage Id"));
	ABL_BIND_DYNAMIC_PROPERTY(Ability, m_OverrideProjectile, TEXT("Override Projectile"));
	ABL_BIND_DYNAMIC_PROPERTY(Ability, m_ShootRotationType, TEXT("ShootRotation Type"));
	ABL_BIND_DYNAMIC_PROPERTY(Ability, m_ShootType, TEXT("Shoot Type"));
	ABL_BIND_DYNAMIC_PROPERTY(Ability, m_ShootDelay, TEXT("Shoot Delay"));
	ABL_BIND_DYNAMIC_PROPERTY(Ability, m_SpawnDelay, TEXT("Spawn Delay"));
	ABL_BIND_DYNAMIC_PROPERTY(Ability, m_ShootDirection, TEXT("Shoot Directionn"));
	ABL_BIND_DYNAMIC_PROPERTY(Ability, m_ProjectileBaseInfo, TEXT("Projectile Info"));
	ABL_BIND_DYNAMIC_PROPERTY(Ability,m_CrossHairAngleConstraint,TEXT("CrossHair AngleConstraint"));
	ABL_BIND_DYNAMIC_PROPERTY(Ability,m_bOnlyAdjustZ,TEXT("Only Adjust Z Dir"));
}

#if WITH_EDITOR

FText UShootProjectileTask::GetDescriptiveTaskName() const
{
	const FText FormatText = LOCTEXT("SPSkillShootProjectileTaskFormat", "{0}: {1} x{2}");
	FString ActorName = TEXT("<null>");
	if (!m_ActorClass.IsNull())
	{
		if (AActor* Actor = Cast<AActor>(m_ActorClass->GetDefaultObject()))
		{
			ActorName = Actor->GetName();
		}
	}

	return FText::FormatOrdered(FormatText, GetTaskName(), FText::FromString(ActorName), m_AmountToSpawn);
}

FText UShootProjectileTask::GetRichTextTaskSummary() const
{
	FTextBuilder StringBuilder;

	StringBuilder.AppendLine(Super::GetRichTextTaskSummary());

	FString ActorName;
	if (m_ActorClassDelegate.IsBound())
	{
		ActorName = FString::Format(
			TEXT(
				"<a id=\"AbleTextDecorators.GraphReference\" style=\"RichText.Hyperlink\" GraphName=\"{0}\">Dynamic</>"),
			{m_ActorClassDelegate.GetFunctionName().ToString()});
	}
	else
	{
		ActorName = FString::Format(
			TEXT(
				"<a id=\"AbleTextDecorators.AssetReference\" style=\"RichText.Hyperlink\" PropertyName=\"m_ActorClass\" Filter=\"Actor\">{0}</>"),
			{m_ActorClass ? m_ActorClass->GetName() : TEXT("NULL")});
	}

	FString AmountToSpawnString;
	if (m_AmountToSpawnDelegate.IsBound())
	{
		AmountToSpawnString = FString::Format(
			TEXT(
				"<a id=\"AbleTextDecorators.GraphReference\" style=\"RichText.Hyperlink\" GraphName=\"{0}\">Dynamic</>"),
			{m_AmountToSpawnDelegate.GetFunctionName().ToString()});
	}
	else
	{
		AmountToSpawnString = FString::Format(
			TEXT(
				"<a id=\"AbleTextDecorators.IntValue\" style=\"RichText.Hyperlink\" PropertyName=\"m_AmountToSpawn\" MinValue=\"1\">{0}</>"),
			{m_AmountToSpawn});
	}

	StringBuilder.AppendLineFormat(
		LOCTEXT("AbleSpawnActorTaskRichFmt", "\t- Actor Class: {0}\n\t- Amount to Spawn: {1}"),
		FText::FromString(ActorName), FText::FromString(AmountToSpawnString));

	return StringBuilder.ToText();
}

EDataValidationResult UShootProjectileTask::IsTaskDataValid(const UAbleAbility* AbilityContext, const FText& AssetName,
                                                            TArray<FText>& ValidationErrors)
{
	EDataValidationResult result = EDataValidationResult::Valid;

	if (m_ActorClass == nullptr)
	{
		ValidationErrors.Add(FText::Format(LOCTEXT("NoClassDefined", "No Actor Class defined: {0}"), AssetName));
		result = EDataValidationResult::Invalid;
	}

	if (m_FireEvent)
	{
		UFunction* function = AbilityContext->GetClass()->FindFunctionByName(TEXT("OnShootProjectileEventBP"));
		if (function == nullptr || function->Script.Num() == 0)
		{
			ValidationErrors.Add(FText::Format(LOCTEXT("OnShootProjectileEventBP_NotFound",
			                                           "Function 'OnShootProjectileEventBP' not found: {0}"), AssetName));
			result = EDataValidationResult::Invalid;
		}
	}

	return result;
}
#endif

void UShootProjectileTask::GetAimingTarget(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
                                           TWeakObjectPtr<AActor>& OutActor,int SpawnIndex) const
{
	TArray<TWeakObjectPtr<AActor>> OutArray;
	TArray<TWeakObjectPtr<AActor>> OutSameTeamArray;
	TArray<TWeakObjectPtr<AActor>> OutDifferentTeamArray;

	switch (m_AimingTarget)
	{
	case EAbleAbilityTargetType::ATT_Self:
	case EAbleAbilityTargetType::ATT_Owner:
	case EAbleAbilityTargetType::ATT_Instigator:
		{
			if (AActor* Actor = GetSingleActorFromTargetType(Context, m_AimingTarget))
			{
				OutArray.Add(Actor);
			}
		}
		break;
	case EAbleAbilityTargetType::ATT_TargetActor:
		{
			const TArray<TWeakObjectPtr<AActor>>& UnfilteredTargets = Context->GetTargetActorsWeakPtr();
			for (const TWeakObjectPtr<AActor>& TempTarget : UnfilteredTargets)
			{
				if (IsTaskValidForActor(TempTarget.Get()))
				{
					OutArray.Add(TempTarget);

					AActor* SPOwnerActor = Context->GetSelfActor();
					AActor* SPTargetActor = TempTarget.Get();

					if (SPOwnerActor && SPTargetActor)
					{
						if (!USPGameLibrary::IsInDifferentTeam(SPOwnerActor, SPTargetActor))
						{
							OutSameTeamArray.Add(TempTarget);
						}
						else
						{
							OutDifferentTeamArray.Add(TempTarget);
						}
					}
				}
			}
		}
		break;
	default:
		break;
	}

	if (m_TargetTeamMode == EProjectileTargetTeamModeType::SameTeam)
	{
		if (OutSameTeamArray.Num() > 0)
		{
			if (m_AimingTarget == EAbleAbilityTargetType::ATT_TargetActor && !m_SoleTarget)
			{
				if (SpawnIndex < OutSameTeamArray.Num())
				{
					OutActor = OutSameTeamArray[SpawnIndex];
				}
			}
			else
			{
				OutActor = OutSameTeamArray.Last();
			}
		}
	}
	else if (m_TargetTeamMode == EProjectileTargetTeamModeType::DifferentTeam)
	{
		if (OutDifferentTeamArray.Num() > 0)
		{
			if (m_AimingTarget == EAbleAbilityTargetType::ATT_TargetActor && !m_SoleTarget)
			{
				if (SpawnIndex < OutDifferentTeamArray.Num())
				{
					OutActor = OutDifferentTeamArray[SpawnIndex];
				}
			}
			else
			{
				OutActor = OutDifferentTeamArray.Last();
			}
		}
	}
	else
	{
		if (OutArray.Num() > 0)
		{
			if (m_AimingTarget == EAbleAbilityTargetType::ATT_TargetActor && !m_SoleTarget)
			{
				if (SpawnIndex < OutArray.Num())
				{
					OutActor = OutArray[SpawnIndex];
				}
			}
			else
			{
				OutActor = OutArray[0];
			}
		}
	}
}

void UShootProjectileTask::SpawnProjectiles(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	UWorld* World = Context->GetWorld();
	if (!World)
		return;

	UShootProjectileTaskScratchPad* ScratchPad = Cast<UShootProjectileTaskScratchPad>(Context->GetScratchPadForTask(this));
	if(ScratchPad == nullptr)
		return;

	ApplyShotSpread(Context);

	// 读取通用信息
	int32 SkillId = Context->GetAbilityId();
	int32 SkillUniqueID = Context->GetAbilityUniqueID();
	FSPSkillInfo Info;
	USPAbilityFunctionLibrary::GetAbilityInfo(Context->GetWorld(), SkillId, Info);
	TArray<int32> DamageIds = Info.DamageIds;
	int DamageId = DamageIds.IsValidIndex(m_DamageIndex) ? DamageIds[m_DamageIndex] : -1;

	if(DamageId < 0) DamageId = m_DamageId;
	bool bShouldFixTarget = Context->GetIntParameter("GMFixedTarget") == 1;

	int NumToSpawn = ABL_GET_DYNAMIC_PROPERTY_VALUE(Context, m_AmountToSpawn);

	if (m_bSpecifyTargetIndex)
	{
		int32 TargetIndex = m_TargetIndex;
		if (m_bClampTargetIndex && ScratchPad->OutActorsNum)
		{
			TargetIndex = FMath::Clamp(TargetIndex, 0, ScratchPad->OutActorsNum - 1);
		}
		
		if (0 <= TargetIndex && TargetIndex < ScratchPad->OutActorsNum)
		{
			SpawnProjectilesForTarget(Context, Info, TargetIndex, NumToSpawn);
		}
	}
	else
	{
		for (int32 i = 0; i < ScratchPad->OutActorsNum; ++i)
		{
			SpawnProjectilesForTarget(Context, Info, i, NumToSpawn);
		}
	}

	if (m_bLoopShoot)
	{
		ScratchPad->CurrentShootTimes++;
		ScratchPad->CurTimeStamp = Context->GetCurrentTime();
	}
}

void UShootProjectileTask::SpawnProjectilesForTarget(const TWeakObjectPtr<const UAbleAbilityContext>& Context, const FSPSkillInfo& SkillInfo, int32 TargetIndex, int32 NumToSpawn /*= 1*/) const
{
	UWorld* World = Context->GetWorld();
	if (!World)
		return;

	UShootProjectileTaskScratchPad* ScratchPad = Cast<UShootProjectileTaskScratchPad>(Context->GetScratchPadForTask(this));
	if (ScratchPad == nullptr)
		return;

	// 读取通用信息
	int32 SkillId = Context->GetAbilityId();
	int32 SkillUniqueID = Context->GetAbilityUniqueID();
	TArray<int32> DamageIds = SkillInfo.DamageIds;
	int DamageId = DamageIds.IsValidIndex(m_DamageIndex) ? DamageIds[m_DamageIndex] : -1;
	if(DamageId < 0) DamageId = m_DamageId;

	bool bShouldFixTarget = Context->GetIntParameter("GMFixedTarget") == 1;

	FShootProjectileTaskSpawnInfo SpawnInfo = ScratchPad->SpawnInfos[TargetIndex];
	// Go through our spawns.
	for (int32 SpawnIndex = 0; SpawnIndex < NumToSpawn; ++SpawnIndex)
	{
		SpawnInfo.SpawnParams.Name = MakeUniqueObjectName(World, SpawnInfo.ActorClass);

		if (m_ProjectileBaseInfo.NetPolicy == EProjectileNetPolicy::LocalOnly && USPGameLibrary::IsDedicatedServer())
		{
			continue;
		}

		FTransform SpawnTransform = GetSpawnTransform(SpawnInfo, SpawnIndex);

#if !(UE_BUILD_SHIPPING)
		if (IsVerbose())
		{
			PrintVerbose(Context, FString::Printf(
				TEXT("Spawning Actor %s using Transform %s."), *SpawnInfo.ActorClass->GetName(),
				*SpawnTransform.ToString()));
		}
#endif

		RefreshByWeaponLocation(Context, SpawnTransform);

		ASPProjectileBase* SpawnedActor = nullptr;

		TWeakObjectPtr<AActor> TargetActor;
		GetAimingTarget(Context, TargetActor, TargetIndex);
		if (!IsValid(TargetActor.Get()) && !m_SpawnWhenTargetEmpty)
		{
			return;
		}

		if (IsSpawnLocally(m_ProjectileBaseInfo.NetPolicy))
		{
			// 双端各自生成, 需要修改UE默认Spawn命名规范
			if (m_ProjectileBaseInfo.NetPolicy == EProjectileNetPolicy::Independent)
			{
				SpawnInfo.SpawnParams.NameMode = FActorSpawnParameters::ESpawnActorNameMode::Requested;
			}

			SpawnedActor = World->SpawnActor<ASPProjectileBase>(
				SpawnInfo.ActorClass, SpawnTransform, SpawnInfo.SpawnParams);
			if (!SpawnedActor)
			{
				MOE_SP_ABILITY_ERROR(TEXT("Failed to spawn Projectile %s using Transform %s."), *SpawnInfo.ActorClass->GetName(),
					*SpawnTransform.ToString());
				return;
			}
			AddIgnoreActors(Context, SpawnedActor);

			SpawnedActor->SetProjectileInfo(SkillId, DamageId, bShouldFixTarget, SkillUniqueID);

			int damage = Context->GetIntParameter("Damage");
			if (m_bIsRealDamage && damage > 0)
			{
				int damageSourceType = Context->GetIntParameter("DamageSourceType");
				int damageSourceID = Context->GetIntParameter("DamageSourceID");
				SpawnedActor->SetProjectileRealDamageInfo(m_bIsRealDamage, damage * (m_RealDamageRatio / 100), damageSourceType, damageSourceID);
			}
			
			if (m_OverrideProjectile)
			{
				SpawnedActor->InitializeProjectile(SpawnInfo.SpawnParams.Owner, m_ShootRotationType, m_ShootRotationTypeBackUp,
					GetProjectileDirection(Context, SpawnTransform.GetLocation()),
					m_ProjectileBaseInfo, m_CrossHairAngleConstraint, m_bOnlyAdjustZ);
			}
			else
			{
				SpawnedActor->OverrideMovementInfo();
				SpawnedActor->SetOwner(SpawnInfo.SpawnParams.Owner);
			}
			
			bool bTargetSet = ApplyWeaponTarget(Context, SpawnedActor);
			if (bTargetSet == false)
			{
				SpawnedActor->SetTarget(TargetActor.Get(), m_AimingTargetSocket);
			}

			CaptureOverrideShootDirection(Context, SpawnedActor);

			JudgeShootType(SpawnedActor);

			if (m_DestroyAtEnd)
			{
				if (ScratchPad)
				{
					ScratchPad->SpawnedActors.Add(SpawnedActor);
				}
			}
			if (m_FireEvent)
			{
#if !(UE_BUILD_SHIPPING)
				if (IsVerbose())
				{
					PrintVerbose(Context, FString::Printf(
						TEXT(
							"Calling OnShootProjectileEvent with event name %s, Spawned Actor %s and Spawn Index %d."),
						*m_Name.ToString(), *SpawnedActor->GetName(), SpawnIndex));
				}
#endif
				// Context->GetAbility()->OnShootProjectileEventBP(Context.Get(), m_Name, SpawnedActor, SpawnIndex);
			}
		}
		else
		{
			// 联机客户端
			auto MoeChar = GetPlayer(Context);
			if (MoeChar)
			{
				FVector ShootDirection = GetProjectileDirection(Context, SpawnTransform.GetLocation());
				if (m_ShootRotationType == ESPShootRotationType::UseCrosshairDirection)
				{
					ShootDirection = GetCrossHairDirection(Context->GetSelfActor());
				}

				AActor* TargetToSet = TargetActor.Get();
				FSPProjectileSpawnContext SpawnContext(SpawnInfo.ActorClass,
					SpawnTransform, MoeChar, Context->GetInstigator(), m_ShootType,
					m_ShootDelay, SkillId, DamageId, SkillUniqueID, ShootDirection,m_OverrideProjectile,
					m_ProjectileBaseInfo, m_ShootRotationType, m_CrossHairAngleConstraint, m_bOnlyAdjustZ,m_ShootRotationTypeBackUp,  ShootDirection,bShouldFixTarget, TargetToSet);
				SpawnContext.bUseCustomRange = bUseCustomRange;
				SpawnContext.WeaponRange = WeaponRange;
				USPGameLibrary::LocalSpawnProjectile(MoeChar->GetMoePlayerController(),SpawnContext);
			}
		}

		// 修改逻辑为直接DS扣除
		if (IsCostItem())
		{
			auto MoeChar = Cast<ASPGameCharacterBase>(Context->GetSelfActor());
			auto CostItemId = m_CostItemId;
			if (m_bIsForCapture)
			{
				CostItemId = GetCaptureCostItemId(Context, CostItemId);
			}
			// 对应Lua中 SPBackpackTypes.ChangeItemReason
			USPGameLibrary::CostItemById(MoeChar, CostItemId, 1, REASON_PROJECTILE_COST);
		}
	}
}

void UShootProjectileTask::LoopSpawnProjectilesByTimeSpanBP_Implementation(const UAbleAbilityContext* Context) const
{
	UShootProjectileTaskScratchPad* ScratchPad = Cast<UShootProjectileTaskScratchPad>(Context->GetScratchPadForTask(this));
	if (!ScratchPad)
	{
		return;
	}

	float CurTimeStamp1 = Context->GetCurrentTime();
	float PrevTimeStamp1 = ScratchPad->CurTimeStamp;
	
	float TimeSpan1 =  CurTimeStamp1 - PrevTimeStamp1;
	UE_LOG(LogSPAbility,Log, TEXT("ShootProjectileTask::LoopSpawnProjectilesByTimeSpanBP_Implementation: TimeSpan %f Already Spawned %d"), TimeSpan1, ScratchPad->CurrentShootTimes)
	// 第一次直接发射
	if (ScratchPad->CurrentShootTimes == 0)
	{
		SpawnProjectiles(Context);
	}
	else
	{
		float CurTimeStamp = Context->GetCurrentTime();
		float PrevTimeStamp = ScratchPad->CurTimeStamp;
		float TimeSpan =  CurTimeStamp - PrevTimeStamp;
		while(TimeSpan >= m_IntervalTime && ScratchPad->CurrentShootTimes < m_TotalShootTimes)
		{
			SpawnProjectiles(Context);
			TimeSpan -= m_IntervalTime;
		}
	}
}


TSubclassOf<AActor> UShootProjectileTask::CaptureOverride(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	auto TargetOwner = GetSingleActorFromTargetType(Context, m_OwnerTargetType);
	TSubclassOf<AActor> Result;
	if(auto SPActor = Cast<ISPActorInterface>(TargetOwner))
	{
		if(auto CaptureComp = SPActor->GetCaptureComponent())
		{
			switch (m_CaptureFunctionType)
			{
			case ECaptureFunctionType::Capture:
				Result = CaptureComp->CurrCaptureShowConfig.ClassCaptureProjectile;
				break;
			case ECaptureFunctionType::Unleash:
				Result = CaptureComp->GetProperUnleashProjectile_Lua();
				break;
			default:
				break;
			}
		}
	}
	return Result;
}

void UShootProjectileTask::CaptureOverrideShootDirection(const TWeakObjectPtr<const UAbleAbilityContext>& Context,ASPProjectileBase* Projectile) const
{
	if (m_bIsForCapture)
	{
		FVector ShootDirection;
		auto TargetOwner = GetSingleActorFromTargetType(Context, m_OwnerTargetType);
		if(auto SPActor = Cast<ISPActorInterface>(TargetOwner))
		{
			// V6BugFix - 只有非1P才会跑这段
			if(!SPActor->IsOwnedByLocalPlayerController())
			{
				if(auto CaptureComp = SPActor->GetCaptureComponent())
				{
					CaptureComp->GetCaptureShootDirection(ShootDirection);

					if(ShootDirection!=FVector::ZeroVector)	// 发射方向变量到了
						{
						CaptureComp->ResetCaptureShoot_Lua();
						}
					else
					{
						// 发射方向没到, 取摄像机方向
						ShootDirection = CaptureComp->GetCameraTransform().GetRotation().GetForwardVector();
					}
					Projectile->ManuallySetShootDirection(ShootDirection);
				}
			}
		}
	}
}

float UShootProjectileTask::GetEndTime() const
{
	float endTime = GetStartTime();
	if (m_DestroyAtEnd)
	{
		endTime = Super::GetEndTime();
	}
	else
	{
		endTime = GetStartTime() + KINDA_SMALL_NUMBER;
	}
	
	if (m_SpawnDelay > 0.0)
	{
		// 留0.1s的时间余量
		endTime = FMath::Max(GetStartTime() + m_SpawnDelay + 0.1,endTime);
	}

	if (m_bLoopShoot)
	{
		float TotalShootTime = m_IntervalTime * (m_TotalShootTimes - 1);
		endTime += FMath::Max(TotalShootTime + 0.2, 0);
		if(m_bManuallySpecifyEndTime && endTime < Super::GetEndTime())
		{
			return Super::GetEndTime();
		}
	}
	
	return endTime;
}

int64 UShootProjectileTask::GetCaptureCostItemId(const TWeakObjectPtr<const UAbleAbilityContext>& Context, int64 CostItemId) const
{
	auto TargetOwner = GetSingleActorFromTargetType(Context, m_OwnerTargetType);
	int64 Result = CostItemId;
	switch (m_CaptureFunctionType)
	{
	case ECaptureFunctionType::Capture:
		if(auto SPActor = Cast<ISPActorInterface>(TargetOwner))
		{
			if(auto CaptureComp = SPActor->GetCaptureComponent())
			{
				Result = CaptureComp->GetCurrCaptureBall();
			}
		}
		break;
	case ECaptureFunctionType::Unleash:
		break;
	case ECaptureFunctionType::Leash:
		break;
	default:
		break;
	}
	return Result;
}

void UShootProjectileTask::AddIgnoreActors(const TWeakObjectPtr<const UAbleAbilityContext>& Context, ASPProjectileBase*& SpawnedActor) const
{
	if (m_InnoreLocationSourceActor)
	{
		UObject* OutUObject = Context->GetUObjectParameter(FName(TEXT("LocationSourceActor")));
		AActor* IgnoreActor = Cast<AActor>(OutUObject);
		if (IsValid(IgnoreActor))
		{
			SpawnedActor->AddIgnoredActor(IgnoreActor);
		}
	}
}

bool UShootProjectileTask::CanApplyShotSpread() const
{
	return m_ShootRotationType == ESPShootRotationType::UseCrosshairDirection && bEnableShotSpread;
}

void UShootProjectileTask::ApplyShotSpread(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	if (bEnableShotSpread)
	{
		ASPGameCharacterBase* Character = GetPlayer(Context);
		if (IsValid(Character))
		{
			if (const ASPWeapon* SPWeapon = Cast<ASPWeapon>(Character->GetCrtEquipWeapon()))
			{
				if (SPWeapon->IsSelfWeapon())
				{
					//1P
					if (USPWeaponFireComponent_PetRange* FireComponent = Cast<USPWeaponFireComponent_PetRange>(SPWeapon->FireComponent))
					{
						FireComponent->ApplyShotSpread();
						
					}
				}
			}
		}
	}
}

TEnumAsByte<EAbleAbilityTaskRealm> UShootProjectileTask::Internal_GetTaskRealm() const
{
	return GetTaskRealmBP();
}

EAbleAbilityTaskRealm UShootProjectileTask::GetTaskRealmBP_Implementation() const
{
	if (m_ShootRotationType == ESPShootRotationType::UseCrosshairDirection || m_ShootRotationType == ESPShootRotationType::UsePlayerAimingPoint || (m_ShootRotationType == ESPShootRotationType::UseSelfRotation && m_ProjectileBaseInfo.NetPolicy == EProjectileNetPolicy::LocalOnly))
	{
		return EAbleAbilityTaskRealm::ATR_Client;
	}
	return m_TaskRealm;
}

bool UShootProjectileTask::IsSeparateLocation() const
{
	return m_AmountToSpawn > 1 && bUseSeparateLocation;
}

void UShootProjectileTask::InitSpawnTargetLocationForTargetActor(const UAbleAbilityContext& Context, int32 TargetIndex, TArray<FTransform>& OutTransform) const
{
	for (auto Location : SpawnLocationList)
	{
		FTransform SpawnTransform;
		if (Location.GetSourceTargetType() == EAbleAbilityTargetType::ATT_TargetActor)
		{
			Location.GetTargetTransform(Context, TargetIndex, SpawnTransform);
		}
		else
		{
			Location.GetTransform(Context, SpawnTransform);
		}
		OutTransform.Add(SpawnTransform);
	}
}

ASPGameCharacterBase* UShootProjectileTask::GetPlayer(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	AActor* Owner = Context->GetSelfActor();
	ASPGameCharacterBase* Character = Cast<ASPGameCharacterBase>(Owner);
	if (bIsRideMode)
	{
		if (!IsValid(Character))
		{
			const ASPGameMonsterBase* Monster = Cast<ASPGameMonsterBase>(Owner);
			if (IsValid(Monster))
			{
				Character = Cast<ASPGameCharacterBase>(Monster->FindRelatedPlayerPawn());
			}
		}
	}
	return Character;
}

FTransform UShootProjectileTask::GetSpawnTransform(FShootProjectileTaskSpawnInfo& SpawnInfo,
                                                          const int32 SpawnIndex) const
{
	if (IsSeparateLocation())
	{
		return SpawnInfo.GetSpawnTransformByIndex(SpawnIndex);
	}
	else
	{
		return SpawnInfo.SpawnTransform;
	}
}

void UShootProjectileTask::RefreshByWeaponLocation(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	FTransform& OutTransform) const
{
	// todo - rectify property name for better readability
	if (m_bUseWeaponLocation)
	{
		ASPGameCharacterBase* Character = GetPlayer(Context);
		if (IsValid(Character))
		{
			ASPWeapon* SPWeapon = Cast<ASPWeapon>(Character->GetCrtEquipWeapon());
			if (SPWeapon != nullptr)
			{
				if (!m_WeaponSocketLocation.IsNone())
				{
					UMeshComponent* MeshComp = SPWeapon->GetWeaponMeshComponent();
					if (MeshComp)
					{
						FTransform WeaponSocketTransform = MeshComp->GetSocketTransform(m_WeaponSocketLocation);
						if(m_bUseWeaponSocketRotation)
						{
							OutTransform.SetRotation(WeaponSocketTransform.GetRotation());
						}
						if(m_bUseWeaponSocketLocation)
						{
							OutTransform.SetLocation(WeaponSocketTransform.GetLocation());
						}
					}
				}
			}
		}
	}
}

bool UShootProjectileTask::ApplyWeaponTarget(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	ASPProjectileBase* SpawnedActor) const
{
	bool bTargetSet = false;
	if (m_bUseWeaponTarget)
	{
		ASPGameCharacterBase* Character = GetPlayer(Context);
		if (IsValid(Character))
		{
			ASPWeapon* SPWeapon = Cast<ASPWeapon>(Character->GetCrtEquipWeapon());
			if (SPWeapon != nullptr)
			{
				if (!m_WeaponTargetSocket.IsNone())
				{
					bTargetSet = true;
					if(IsValid(SpawnedActor))
						SpawnedActor->SetTarget(SPWeapon, m_WeaponTargetSocket);
				}
			}
		}
	}
	return bTargetSet;
}

bool UShootProjectileTask::IsSpawnLocally(EProjectileNetPolicy NetPolicy) const
{
	return USPGameLibrary::IsStandAlone() || USPGameLibrary::IsDedicatedServer()
					|| NetPolicy == EProjectileNetPolicy::LocalOnly
					|| NetPolicy == EProjectileNetPolicy::Independent
					|| USPGameLibrary::IsEditorPreview(GetWorld());
}

bool UShootProjectileTask::IsCostItem() const
{
	return m_bCostItem && (USPGameLibrary::IsDedicatedServer() || USPGameLibrary::IsStandAlone());
}

bool UShootProjectileTask::IsSingleFrameBP_Implementation() const
{
	if (m_bLoopShoot)
	{
		return false;
	}

	return !(m_DestroyAtEnd || m_SpawnDelay > 0.0f);
}

#undef LOCTEXT_NAMESPACE

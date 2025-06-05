// Copyright (c) Extra Life Studios, LLC. All rights reserved.

#include "Game/SPGame/Skill/Task/SPLaserDamageTask.h"

#include "ableAbility.h"
#include "ableSubSystem.h"
#include "Game/SPGame/Skill/Core/SPAbilityFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Particles/ParticleSystemComponent.h"

USPLaserDamageTaskScratchPad::USPLaserDamageTaskScratchPad()
{
}

USPLaserDamageTaskScratchPad::~USPLaserDamageTaskScratchPad()
{
}

USPLaserDamageTask::USPLaserDamageTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	  m_QueryShape(nullptr),
	  m_TickQueryShapeChange(true),
	  m_DamageIndex(0),
	  m_DamageSource(),
	  m_Interval(1.f),
	  m_EffectTemplate(nullptr),
	  m_AttachToSocket(false),
	  m_Scale(1.f),
	  m_DestroyAtEnd(true),
	  m_AddBuffWhenDamage(false),
	  m_BuffID(0),
	  m_BuffLayer(0),
	  m_AddBuffRepeat(false),
	  m_IndexFromSkillCfg(false)
{
}

USPLaserDamageTask::~USPLaserDamageTask()
{
}

FString USPLaserDamageTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPLaserDamageTask");
}

void USPLaserDamageTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);
	OnTaskStartBP(Context.Get());
}

void USPLaserDamageTask::OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const
{

	USPLaserDamageTaskScratchPad* ScratchPad = Cast<USPLaserDamageTaskScratchPad>(
		Context->GetScratchPadForTask(this));
	if (!ScratchPad) return;

	const float IntervalValue = m_Interval;
	ScratchPad->IntervalTimer = IntervalValue;

	DoQuery(Context, false);
	DoDamage(Context);

	if (!UKismetSystemLibrary::IsDedicatedServer(Context))
	{
		PlayParticleEffect(Context);
		RefreshParticleScale(Context);
	}
}

void USPLaserDamageTask::OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context, float deltaTime) const
{
	Super::OnTaskTick(Context, deltaTime);
	OnTaskTickBP(Context.Get(), deltaTime);
}

void USPLaserDamageTask::OnTaskTickBP_Implementation(const UAbleAbilityContext* Context, float deltaTime) const
{

	const float DeltaTimeWithRate = deltaTime;
	USPLaserDamageTaskScratchPad* ScratchPad = Cast<USPLaserDamageTaskScratchPad>(
		Context->GetScratchPadForTask(this));
	if (!ScratchPad) return;

	DoQuery(Context, true);

	ScratchPad->IntervalTimer -= DeltaTimeWithRate;
	if (ScratchPad->IntervalTimer <= 0.f)
	{
		const float IntervalValue = m_Interval;
		ScratchPad->IntervalTimer = IntervalValue;
		DoDamage(Context);
	}

	if (!UKismetSystemLibrary::IsDedicatedServer(Context))
		RefreshParticleScale(Context);
}

void USPLaserDamageTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
                                   const EAbleAbilityTaskResult result) const
{
	Super::OnTaskEnd(Context, result);
	OnTaskEndBP(Context.Get(), result);
}

void USPLaserDamageTask::OnTaskEndBP_Implementation(const UAbleAbilityContext* Context,
                                                    const EAbleAbilityTaskResult result) const
{

	USPLaserDamageTaskScratchPad* ScratchPad = Cast<USPLaserDamageTaskScratchPad>(
		Context->GetScratchPadForTask(this));
	if (!ScratchPad) return;
	ScratchPad->AddBuffActors.Empty();
	ScratchPad->IntervalTimer = 0.f;

	if (!UKismetSystemLibrary::IsDedicatedServer(Context))
		StopParticleEffect(Context);
}

UAbleAbilityTaskScratchPad* USPLaserDamageTask::CreateScratchPad(
	const TWeakObjectPtr<UAbleAbilityContext>& Context) const
{
	if (UAbleAbilityUtilitySubsystem* Subsystem = Context->GetUtilitySubsystem())
	{
		static TSubclassOf<UAbleAbilityTaskScratchPad> ScratchPadClass =
			USPLaserDamageTaskScratchPad::StaticClass();
		return Subsystem->FindOrConstructTaskScratchPad(ScratchPadClass);
	}

	return NewObject<USPLaserDamageTaskScratchPad>(Context.Get());
}

void USPLaserDamageTask::DoQuery(const TWeakObjectPtr<const UAbleAbilityContext>& Context, bool bTickUpdate) const
{
	if (m_QueryShape)
	{
		USPLaserDamageTaskScratchPad* ScratchPad = Cast<USPLaserDamageTaskScratchPad>(
	Context->GetScratchPadForTask(this));
		if (!ScratchPad) return;
		TArray<FAbleQueryResult> Results;
		if (m_TickQueryShapeChange)
		{
			m_QueryShape->DoQuery(Context, Results);	
		}
		else
		{
			if (!bTickUpdate)
			{
				const FTransform OriginTransform = m_QueryShape->DoQuery(Context, Results);
				ScratchPad->OriginTransform = OriginTransform;
			}
			else
			{
				m_QueryShape->DoQuery(Context, Results, ScratchPad->OriginTransform, true);	
			}
		}
		UE_LOG(LogTemp, Log, TEXT("Laser Collision Query Task Result, %i"), Results.Num())
		
		if (Results.Num())
		{
			for (const UAbleCollisionFilter* CollisionFilter : m_Filters)
			{
				// 防止配置上多出了一些None的Filter
				if (!IsValid(CollisionFilter)) continue;
				CollisionFilter->Filter(Context, Results);
			}

			ScratchPad->QueryResults.Empty();

			if (Results.Num())
			{
				ScratchPad->QueryResults.Add(Results[0]);
			}
		}
		else
		{
			ScratchPad->QueryResults.Empty();
		}
	}
}

void USPLaserDamageTask::DoDamage(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	if (!UKismetSystemLibrary::IsStandalone(Context.Get())
		&& !UKismetSystemLibrary::IsDedicatedServer(Context.Get()))
		return;

	USPLaserDamageTaskScratchPad* ScratchPad = Cast<USPLaserDamageTaskScratchPad>(
		Context->GetScratchPadForTask(this));
	if (!ScratchPad) return;

	if (ScratchPad->QueryResults.Num() <= 0) return;

	const int32 SkillId = Context->GetAbilityId();
	const int32 SkillUniqueID = Context->GetAbilityUniqueID();
	int32 DamageIndex = m_DamageIndex;
	FSPSkillInfo SkillInfo;
	USPAbilityFunctionLibrary::GetAbilityInfo(Context->GetWorld(), SkillId, SkillInfo);

	TArray<int32> DamageIds = SkillInfo.DamageIds;
	int32 DamageId = DamageIds.IsValidIndex(DamageIndex) ? DamageIds[DamageIndex] : INDEX_NONE;
	AController* InstigatorController = nullptr;
	if (AActor* InstigatorActor = Context->GetInstigator())
	{
		if (InstigatorActor->IsA<AController>())
			InstigatorController = Cast<AController>(InstigatorActor);
		else if (InstigatorActor->IsA<APawn>())
			InstigatorController = Cast<APawn>(InstigatorActor)->GetController();
	}
	AActor* DamageSource = GetSingleActorFromTargetType(Context, m_DamageSource);

	for (FAbleQueryResult QueryResult : ScratchPad->QueryResults)
	{
		if (!QueryResult.Actor.IsValid()) continue;
		USPGameLibrary::DoDamage(SkillId, DamageId, SkillUniqueID, QueryResult.Actor.Get(), FShootDamageEvent(),
		                         InstigatorController,
		                         DamageSource, TEXT(""));
		DoBuffLogic(Context, DamageSource, QueryResult.Actor.Get());
	}
}

void USPLaserDamageTask::DoBuffLogic(const TWeakObjectPtr<const UAbleAbilityContext>& Context, AActor* SourceActor, AActor* TargetActor) const
{
	if(TargetActor == nullptr || !m_AddBuffWhenDamage)
		return;

	if(!m_AddBuffRepeat)
	{
		//Buff只加一次
		USPLaserDamageTaskScratchPad* ScratchPad = Cast<USPLaserDamageTaskScratchPad>(
			Context->GetScratchPadForTask(this));
		if(!ScratchPad)
			return;
		if (ScratchPad->AddBuffActors.Contains(TargetActor))
			return;
		const bool AddSuccess = USPGameLibrary::AddBuffByDamageSkill(Context->GetAbilityId(),m_BuffID, m_BuffLayer, m_IndexFromSkillCfg, SourceActor, TargetActor, GetWorld());
		if(AddSuccess)
		{
			ScratchPad->AddBuffActors.Add(TargetActor);
		}
	}
	else
	{
		USPGameLibrary::AddBuffByDamageSkill(Context->GetAbilityId(),m_BuffID, m_BuffLayer, m_IndexFromSkillCfg, SourceActor, TargetActor, GetWorld());
	}
}


void USPLaserDamageTask::PlayParticleEffect(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	if (!m_EffectTemplate)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Particle System set for USPLaserDamageTask in Ability [%s]"),
		       *GetNameSafe(Context->GetAbility()));
		return;
	}

	FTransform OffsetTransform(FTransform::Identity);
	FTransform SpawnTransform = OffsetTransform;

	FAbleAbilityTargetTypeLocation Location = m_Location;
	UParticleSystem* EffectTemplate = m_EffectTemplate;

	float Scale = m_Scale;
	TWeakObjectPtr<UParticleSystemComponent> SpawnedEffect = nullptr;

	USPLaserDamageTaskScratchPad* ScratchPad = nullptr;
	if (m_DestroyAtEnd)
	{
		ScratchPad = Cast<USPLaserDamageTaskScratchPad>(Context->GetScratchPadForTask(this));
		if (ScratchPad) ScratchPad->SpawnedEffect = nullptr;
	}

	TWeakObjectPtr<AActor> Target = GetSingleActorFromTargetType(Context, ATT_Self);
	if (!Target.IsValid()) return;
	
	SpawnTransform.SetIdentity();

	Location.GetTransform(*Context, SpawnTransform);

	float DistanceScaleX = CalculateParticleScale(Context, true, SpawnTransform);
	FVector emitterScale(DistanceScaleX, Scale, Scale);
	SpawnTransform.SetScale3D(emitterScale);

	//Set Pool Method
	EPSCPoolMethod PoolMethod = EPSCPoolMethod::AutoRelease;
	if (m_DestroyAtEnd)
	{
		PoolMethod = EPSCPoolMethod::ManualRelease;
	}
	
	if (m_AttachToSocket)
	{
		USceneComponent* AttachComponent = Target->FindComponentByClass<USkeletalMeshComponent>();
		if (!AttachComponent) AttachComponent = Target->FindComponentByClass<USceneComponent>();
		
		if (EffectTemplate)
		{
			FRotator AttachRotation = Location.GetRotation();
			if (Location.NeedModifyRotation(*Context))
			{
				// world transform
				// transform to socket bone local transform
				FTransform SocketTransform = OffsetTransform;
				if (Location.GetSocketTransform(*Context, SocketTransform))
				{
					FQuat SocketLocationQuat = SocketTransform.InverseTransformRotation(SpawnTransform.GetRotation());
					AttachRotation = SocketLocationQuat.Rotator();
				}
			}
			if (Location.NeedSpawnTraceToGround())
			{
				// transform world location to bone local
				FTransform SocketTransform = OffsetTransform;
				if (Location.GetSocketTransform(*Context, SocketTransform))
				{
					const FVector LocalPosition = SocketTransform.InverseTransformPosition(SpawnTransform.GetLocation());
					SpawnedEffect = UGameplayStatics::SpawnEmitterAttached(EffectTemplate, AttachComponent, Location.GetSocketName(), LocalPosition, AttachRotation, SpawnTransform.GetScale3D(), EAttachLocation::KeepRelativeOffset, true, PoolMethod);
				}
				else
				{
					SpawnedEffect = UGameplayStatics::SpawnEmitterAttached(EffectTemplate, AttachComponent, Location.GetSocketName(), Location.GetOffset(), AttachRotation, SpawnTransform.GetScale3D(), EAttachLocation::KeepRelativeOffset, true, PoolMethod);   
				}
			}
			else
			{
				SpawnedEffect = UGameplayStatics::SpawnEmitterAttached(EffectTemplate, AttachComponent, Location.GetSocketName(), Location.GetOffset(), AttachRotation, SpawnTransform.GetScale3D(), EAttachLocation::KeepRelativeOffset, true, PoolMethod);	
			}
			SpawnedEffect->SetWorldScale3D(SpawnTransform.GetScale3D());
		}
	}
	else
	{
		if (EffectTemplate)
		{
			SpawnedEffect = UGameplayStatics::SpawnEmitterAtLocation(
				Target->GetWorld(), EffectTemplate, SpawnTransform, true, PoolMethod);
			SpawnedEffect->SetWorldScale3D(SpawnTransform.GetScale3D());
		}
	}

	if (m_DestroyAtEnd && ScratchPad)
	{
		if (SpawnedEffect.IsValid())
		{
			ScratchPad->SpawnedEffect = SpawnedEffect;
		}
	}
}

void USPLaserDamageTask::StopParticleEffect(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	if (m_DestroyAtEnd && Context.IsValid())
	{
		USPLaserDamageTaskScratchPad* ScratchPad = Cast<USPLaserDamageTaskScratchPad>(
			Context->GetScratchPadForTask(this));
		if (!ScratchPad) return;

		if (ScratchPad->SpawnedEffect.IsValid())
		{
			ScratchPad->SpawnedEffect->bAutoDestroy = true;
			ScratchPad->SpawnedEffect->ReleaseToPool();
			ScratchPad->SpawnedEffect->DeactivateSystem();
		}
	}
}

void USPLaserDamageTask::RefreshParticleScale(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	USPLaserDamageTaskScratchPad* ScratchPad = Cast<USPLaserDamageTaskScratchPad>(
		Context->GetScratchPadForTask(this));
	if (!ScratchPad) return;

	if (ScratchPad->SpawnedEffect.IsValid())
	{
		const float DistanceScaleX = CalculateParticleScale(Context, false);
		auto WorldScale = ScratchPad->SpawnedEffect->GetComponentScale();
		WorldScale.X = DistanceScaleX;
		ScratchPad->SpawnedEffect->SetWorldScale3D(WorldScale);
		auto ModifiedScale = ScratchPad->SpawnedEffect->GetComponentScale();
		UE_LOG(LogTemp, Log, TEXT("Modify Scale = [%f],[%f],[%f]"), ModifiedScale.X, ModifiedScale.Y, ModifiedScale.Z);
	}
}

float USPLaserDamageTask::CalculateParticleScale(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
                                                 const bool SpecialTransform, const FTransform Transform) const
{
	if (m_ParticleNormalizeLength <= 0.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("USPLaserDamageTask Particle in Ability [%s] Normalize Length Invalid !"),
		       *GetNameSafe(Context->GetAbility()));
		return 1.f;
	}

	USPLaserDamageTaskScratchPad* ScratchPad = Cast<USPLaserDamageTaskScratchPad>(
		Context->GetScratchPadForTask(this));
	if (!ScratchPad) return 1.f;

	if (ScratchPad->QueryResults.Num())
	{
		if (ScratchPad->SpawnedEffect.IsValid())
		{
			const FVector SourceLocation = SpecialTransform
				                               ? Transform.GetLocation()
				                               : ScratchPad->SpawnedEffect->GetComponentLocation();
			const FVector SourceDirection = SpecialTransform
				                                ? Transform.GetRotation().GetForwardVector()
				                                : ScratchPad->SpawnedEffect->GetForwardVector();

			const FVector TargetLocation = ScratchPad->QueryResults[0].GetLocation();
			auto ProjectPoint = SourceLocation + ((TargetLocation - SourceLocation) | SourceDirection) *
				SourceDirection;

			UE_LOG(LogTemp, Log, TEXT("SourceLocation = [%f],[%f],[%f]"), SourceLocation.X, SourceLocation.Y,
			       SourceLocation.Z);
			UE_LOG(LogTemp, Log, TEXT("TargetLocation = [%f],[%f],[%f]"), TargetLocation.X, TargetLocation.Y,
			       TargetLocation.Z);
			UE_LOG(LogTemp, Log, TEXT("ProjectPoint = [%f],[%f],[%f]"), ProjectPoint.X, ProjectPoint.Y, ProjectPoint.Z);
			if (ScratchPad->QueryResults[0].Actor.IsValid())
			{
				UE_LOG(LogTemp, Log, TEXT("USPLaserDamageTask CalculateParticleScale Target Object Name = %s"), *ScratchPad->QueryResults[0].Actor->GetName());
			}
			const float Distance = UKismetMathLibrary::Vector_Distance(SourceLocation, ProjectPoint);
			UE_LOG(LogTemp, Log, TEXT("Distance = [%f]"), Distance);
			return FMath::Min(Distance / m_ParticleNormalizeLength, m_ParticleMaxLength / m_ParticleNormalizeLength);
		}
	}

	return m_ParticleMaxLength / m_ParticleNormalizeLength;
}

bool USPLaserDamageTask::IsSingleFrameBP_Implementation() const { return false; }

EAbleAbilityTaskRealm USPLaserDamageTask::GetTaskRealmBP_Implementation() const { return ATR_ClientAndServer; }

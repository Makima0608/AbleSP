// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SPGame/Skill/Task/SPCharacterTurnToTask.h"

#include "ableSubSystem.h"
#include "Game/SPGame/Character/SPGameCharacterBase.h"
#include "Game/SPGame/Character/SPGameCharInputComponent.h"
#include "Game/SPGame/Utils/SPGameLibrary.h"
#include "MoeGameplay/Character/Component/MoeCharAttributeComponent.h"
#include "MoeGameplay/Monster/ACharacterBase.h"
#include "Tasks/ableTurnToTask.h"
#include "Game/SPGame/Weapon/SPWeapon.h"

#define LOCTEXT_NAMESPACE "AbleAbilityTask"

USPCharacterTurnToTaskScratchPad::USPCharacterTurnToTaskScratchPad(): ChangeYawBuff(nullptr)
{
}

USPCharacterTurnToTaskScratchPad::~USPCharacterTurnToTaskScratchPad()
{
}

UAbleAbilityTargetingFilterSameTeam::UAbleAbilityTargetingFilterSameTeam(
	const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
}

UAbleAbilityTargetingFilterSameTeam::~UAbleAbilityTargetingFilterSameTeam()
{
}

void UAbleAbilityTargetingFilterSameTeam::Filter(UAbleAbilityContext& Context,
                                                    const UAbleTargetingBase& TargetBase) const
{
	AActor* SelfActor = Context.GetSelfActor();
	if(SelfActor == nullptr)
		return;
	ISPActorInterface* SelfSPActor= Cast<ISPActorInterface>(SelfActor);
	if(SelfSPActor == nullptr)
		return;
	//不索敌友军
	Context.GetMutableTargetActors().RemoveAll([SelfActor](const TWeakObjectPtr<AActor>& LHS)
	{
		if(LHS!=nullptr)
		{
			return !USPGameLibrary::IsInDifferentTeam(LHS.Get(), SelfActor);
		}
		return false;
	});
}

UAbleAbilityTargetingFilterDead::UAbleAbilityTargetingFilterDead(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
}

UAbleAbilityTargetingFilterDead::~UAbleAbilityTargetingFilterDead()
{
}

void UAbleAbilityTargetingFilterDead::Filter(UAbleAbilityContext& Context, const UAbleTargetingBase& TargetBase) const
{
	Context.GetMutableTargetActors().RemoveAll([](const TWeakObjectPtr<AActor>& LHS)
	{
		if(LHS!=nullptr)
		{
			if(auto SPLHS = Cast<AAMonsterCharacter>(LHS))
			{
				return SPLHS!=nullptr&&SPLHS->GetIsDead();
			}
			// 过滤死亡玩家
			else if (auto SPGameCharacterLHS = Cast<ASPGameCharacterBase>(LHS))
			{
				return SPGameCharacterLHS != nullptr && SPGameCharacterLHS->GetIsDead();
			}
		}
		return false;
	});
}

USPCharacterTurnToTask::USPCharacterTurnToTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, m_LockFaceToFirstFrame(false)
	, m_RotateByWeaponTrace(false)
	, m_TurnToTargetRotationOnInterrupt(false)
	, m_TurnToTargetRotationOffset(FRotator::ZeroRotator)
	, m_TurnToTargetRotationOffsetTime(0.0f)
{
}

FString USPCharacterTurnToTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPCharacterTurnToTask");
}

void USPCharacterTurnToTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	OnTaskStartBP_Override(Context.Get());
}

void USPCharacterTurnToTask::OnTaskStartBP_Override_Implementation(const UAbleAbilityContext* Context) const
{
    USPCharacterTurnToTaskScratchPad* ScratchPad = Cast<USPCharacterTurnToTaskScratchPad>(Context->GetScratchPadForTask(this));
	if (!ScratchPad) return;

	if(ScratchPad == nullptr)
		return;
	
	ScratchPad->InProgressTurn.Empty();
	ScratchPad->TurningBlend = m_Blend;
	ScratchPad->ChangeYawBuff = nullptr;
	ScratchPad->TurnToRotationOffsetTime = 0.0f;
	if (m_UseTaskDurationAsBlendTime)
	{
		ScratchPad->TurningBlend.SetBlendTime(GetDuration());
	}

	//避免配置的时间大于Task的持续时间
	ScratchPad->TurnToRotationOffsetBaseTime = m_TurnToTargetRotationOffsetTime;
	if (m_TurnToTargetRotationOffsetTime > GetDuration())
	{
		ScratchPad->TurnToRotationOffsetBaseTime = GetDuration();
	}

	AActor* TargetActor = GetSingleActorFromTargetType(Context, m_RotationTarget.GetValue());

	if(TargetActor!=nullptr)
	{
		TArray<TWeakObjectPtr<AActor>> TaskTargets;
		GetActorsForTask(Context, TaskTargets);

		for (TWeakObjectPtr<AActor>& TurnTarget : TaskTargets)
		{
			FRotator TargetRotation = GetTargetRotation(Context, TurnTarget.Get(), TargetActor);
#if !(UE_BUILD_SHIPPING)
			if (IsVerbose())
			{
				PrintVerbose(Context, FString::Printf(TEXT("Setting up turning for Actor %s with a Target turn of %s."), *TurnTarget->GetName(), *TargetRotation.ToCompactString()));
			}
#endif
			ScratchPad->InProgressTurn.Add(FSPTurnToTaskEntry(TurnTarget.Get(), TargetRotation));
			CharacterSetMeshYaw(TurnTarget.Get(),TargetRotation,ScratchPad);
		}
	}
}

void USPCharacterTurnToTask::OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context, float deltaTime) const
{
	OnTaskTickBP_Override(Context.Get(), deltaTime);
}

void USPCharacterTurnToTask::OnTaskTickBP_Override_Implementation(const UAbleAbilityContext* Context,
	float deltaTime) const
{
    USPCharacterTurnToTaskScratchPad* ScratchPad = Cast<USPCharacterTurnToTaskScratchPad>(Context->GetScratchPadForTask(this));
	if (!ScratchPad) return;

	if(ScratchPad == nullptr)
		return;

	ScratchPad->TurningBlend.Update(deltaTime);
	const float BlendingValue = ScratchPad->TurningBlend.GetBlendedValue();
	AActor* TargetActor = GetSingleActorFromTargetType(Context, m_RotationTarget.GetValue());

	for (FSPTurnToTaskEntry& Entry : ScratchPad->InProgressTurn)
	{
		if (Entry.Actor.IsValid()&& TargetActor)
		{
			if (m_TrackTarget)
			{
				// Update our Target rotation.
				FRotator CurrentTurnToRotation = GetTargetRotation(Context, Entry.Actor.Get(), TargetActor);
				FRotator NewRotation = CurrentTurnToRotation;
				if (m_RotateByWeaponTrace)
				{
					NewRotation = CurrentTurnToRotation + m_TurnToTargetRotationOffset;
					if (ScratchPad->TurnToRotationOffsetBaseTime > 0.0f)
					{
						ScratchPad->TurnToRotationOffsetTime += deltaTime;
						float lerpAlpha = ScratchPad->TurnToRotationOffsetTime / ScratchPad->TurnToRotationOffsetBaseTime;
						lerpAlpha = FMath::Clamp(lerpAlpha, 0.0f, 1.0f);
						NewRotation = FMath::Lerp(CurrentTurnToRotation, NewRotation, lerpAlpha);
					}
				}
				Entry.Target = NewRotation;
			}

			FRotator MeshRotation(0.f);
			AMoeGameCharacter* MoeGameCharacter = Cast<AMoeGameCharacter>(Entry.Actor.Get());
			if (MoeGameCharacter)
			{
				MeshRotation = MoeGameCharacter->GetMesh()->GetComponentRotation();
			}
			FRotator LerpedRotation = FMath::Lerp(MeshRotation, Entry.Target, BlendingValue);

			if(m_LockFaceToFirstFrame)
			{
				LerpedRotation = Entry.Target;
			}
			
			CharacterSetMeshYaw(Entry.Actor.Get(),LerpedRotation,ScratchPad);
#if !(UE_BUILD_SHIPPING)
			if (IsVerbose())
			{
				PrintVerbose(Context, FString::Printf(TEXT("Setting Actor %s rotation to %s ."), *Entry.Actor->GetName(), *LerpedRotation.ToCompactString()));
			}
#endif

			
		}
	}
}

void USPCharacterTurnToTask::CharacterSetMeshYaw(AActor* TargetActor,const FRotator& Rotation,USPCharacterTurnToTaskScratchPad* ScratchPad) const
{
	if(TargetActor == nullptr || ScratchPad == nullptr)
		return;

	AMoeGameCharacter* Character = Cast<AMoeGameCharacter>(TargetActor);

	float TargetYaw = Rotation.Yaw - 90;
	
	if(Character && Character->IsLocallyControlled())
	{
		if (m_bUseFastTurn)
		{
			auto InputComp = Cast<USPGameCharInputComponent>(Character->GetCharInputComponent());
			if (InputComp)
			{
				InputComp->SetActorYaw(Rotation.Yaw);
			}
		}

		else
		{
			auto BuffComponent = Character->GetCharBuffComponent();
			if(BuffComponent == nullptr)
			{
				return;
			}
			if(ScratchPad->ChangeYawBuff==nullptr)
			{
				ScratchPad->ChangeYawBuff = BuffComponent->ChangeFloatAttribute(Character->GetCharAttributeComponent()->MeshTargetYaw,1, TargetYaw,nullptr,0,TEXT("SPCharacterTurnTo"));
				BuffComponent->ForceRefreshBuff(ScratchPad->ChangeYawBuff);
			}else
			{
				ScratchPad->ChangeYawBuff->ChangeOffsetValue(TargetYaw);
			}

			//Character->GetMesh()->SetWorldRotation(Rotation);
		}
	}
}

void USPCharacterTurnToTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	const EAbleAbilityTaskResult result) const
{

	OnTaskEndBP_Override(Context.Get(), result);
}

void USPCharacterTurnToTask::OnTaskEndBP_Override_Implementation(const UAbleAbilityContext* Context,
	const EAbleAbilityTaskResult result) const
{
    if (!Context)
	{
		return;
	}

	USPCharacterTurnToTaskScratchPad* ScratchPad = Cast<USPCharacterTurnToTaskScratchPad>(Context->GetScratchPadForTask(this));
	if (!ScratchPad) return;

	if(ScratchPad == nullptr)
		return;
	
	for (const FSPTurnToTaskEntry& Entry : ScratchPad->InProgressTurn)
	{
		if (Entry.Actor.IsValid())
		{
#if !(UE_BUILD_SHIPPING)
			if (IsVerbose())
			{
				PrintVerbose(Context, FString::Printf(TEXT("Setting Actor %s rotation to %s ."), *Entry.Actor->GetName(), *Entry.Target.ToCompactString()));
			}
#endif
			CharacterSetMeshYaw(Entry.Actor.Get(),Entry.Target,ScratchPad);
			AMoeGameCharacter* Character = Cast<AMoeGameCharacter>(Entry.Actor);
			if(Character!=nullptr && Character->GetCharBuffComponent())
			{
				Character->GetCharBuffComponent()->RemoveBuffByTag(TEXT("SPCharacterTurnTo"));
				if (m_TurnToTargetRotationOnInterrupt)
				{
					Character->GetMesh()->SetWorldRotation(FRotator(Entry.Target.Pitch, Entry.Target.Yaw - 90, Entry.Target.Roll));
				}
			}
		}
	}
}

TStatId USPCharacterTurnToTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPCharacterTurnToTask, STATGROUP_USPAbility);
}

UAbleAbilityTaskScratchPad* USPCharacterTurnToTask::CreateScratchPad(
	const TWeakObjectPtr<UAbleAbilityContext>& Context) const
{
	if (UAbleAbilityUtilitySubsystem* Subsystem = Context->GetUtilitySubsystem())
	{
		static TSubclassOf<UAbleAbilityTaskScratchPad> ScratchPadClass = USPCharacterTurnToTaskScratchPad::StaticClass();
		return Subsystem->FindOrConstructTaskScratchPad(ScratchPadClass);
	}

	return NewObject<USPCharacterTurnToTaskScratchPad>(Context.Get());
}

FRotator USPCharacterTurnToTask::GetTargetRotation(const TWeakObjectPtr<const UAbleAbilityContext>& Context, const AActor* Source, const AActor* Destination) const
{
	if (m_RotateByWeaponTrace)
	{
		return GetTargetRotationByWeaponTrace(Source);
	}
	return Super::GetTargetRotation(Context, Source, Destination);
}

FRotator USPCharacterTurnToTask::GetTargetRotationByWeaponTrace(const AActor* Source) const
{
	FVector SourceLocation = Source->GetActorLocation();
	FRotator SourceRotation = Source->GetActorRotation();

	if (USkeletalMeshComponent* MeshComponent = Source->FindComponentByClass<USkeletalMeshComponent>())
	{
		SourceLocation = MeshComponent->GetComponentLocation();
		SourceRotation = MeshComponent->GetComponentRotation();
	}

	if (AMoeGameCharacter* Character = Cast<AMoeGameCharacter>(const_cast<AActor*>(Source)))
	{
		if (ASPWeapon* Weapon = Cast<ASPWeapon>(Character->GetCrtEquipWeapon()))
		{
			FQuat SourceQuat = Weapon->GetFireStartTraceQuaternionByLocation(SourceLocation, Character->GetActorForwardVector());
			SourceRotation = SourceQuat.Rotator();
		}
	}

	return SourceRotation;
}

#undef LOCTEXT_NAMESPACE

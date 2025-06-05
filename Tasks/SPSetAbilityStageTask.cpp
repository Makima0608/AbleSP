// Copyright (c) Extra Life Studios, LLC. All rights reserved.

#include "Game/SPGame/Skill/Task/SPSetAbilityStageTask.h"

#include "ableAbility.h"
#include "ableAbilityBlueprintLibrary.h"
#include "ableSubSystem.h"
#include "Game/SPGame/Utils/SPGameLibrary.h"

#include "Game/SPGame/Monster/AnimInstance/SPMonsterAnimInstance.h"

#include "Animation/AnimSequenceBase.h"
#include "Animation/AnimSequence.h"
#include "Game/SPGame/Character/SPCharacterMovementComponent.h"
#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

USPSetAbilityStageScratchPad::USPSetAbilityStageScratchPad()
{
}

USPSetAbilityStageScratchPad::~USPSetAbilityStageScratchPad()
{
}

USPSeAbilityStageTask::USPSeAbilityStageTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	  , Stage(ESPSkillStage::Start), Last()
	  , m_OverrideVisibilityBasedAnimTick(true)
      , m_VisibilityBasedAnimTick(EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones)
{
}

USPSeAbilityStageTask::~USPSeAbilityStageTask()
{
}

FString USPSeAbilityStageTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPSetAbilityStageTask");
}

void USPSeAbilityStageTask::ResetScratchPad(UAbleAbilityTaskScratchPad* ScratchPad) const
{
	Super::ResetScratchPad(ScratchPad);
	if (USPSetAbilityStageScratchPad* CustomScratchPad = Cast<USPSetAbilityStageScratchPad>(ScratchPad))
	{
		CustomScratchPad->AnimInstanceCache = nullptr;
		CustomScratchPad->CachedVisibilityBasedAnimTickOptionMap.Empty();
	}
}

void USPSeAbilityStageTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);
	OnTaskStartBP(Context.Get());
}

void USPSeAbilityStageTask::OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const
{

	USPSetAbilityStageScratchPad* ScratchPad = Cast<USPSetAbilityStageScratchPad>(
		Context->GetScratchPadForTask(this));
	if (!ScratchPad) return;
	ScratchPad->CachedVisibilityBasedAnimTickOptionMap.Empty();
	
	const ESPSkillStage AnimStage = Stage;
	if (AnimStage >= ESPSkillStage::Max) return;

	const AActor* Target = Context->GetOwner();
	if (!IsValid(Target)) return;

	USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(Target->GetComponentByClass(
		USkeletalMeshComponent::StaticClass()));
	if (!SkeletalMeshComponent) return;

	UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance();
	if (!AnimInstance || !AnimInstance->IsA(USPMonsterAnimInstance::StaticClass())) return;
	USPMonsterAnimInstance* MonsterAnimInstance = Cast<USPMonsterAnimInstance>(AnimInstance);
	if (MonsterAnimInstance)
	{
		ScratchPad->AnimInstanceCache = MonsterAnimInstance;
		MonsterAnimInstance->SetSkillAnimStage(AnimStage, true);
		MonsterAnimInstance->SetSkillType(ESPSkillType::FarSkill);
	}
	if (m_OverrideVisibilityBasedAnimTick)
	{
		// Cached origin VisibilityBasesAnimTickOption for restore when animation ended.
		if (IsValid(SkeletalMeshComponent))
		{
			ScratchPad->CachedVisibilityBasedAnimTickOptionMap.Add(SkeletalMeshComponent, SkeletalMeshComponent->VisibilityBasedAnimTickOption);
			SkeletalMeshComponent->VisibilityBasedAnimTickOption = m_VisibilityBasedAnimTick;
			if (UKismetSystemLibrary::IsDedicatedServer(this))
			{
				UAbleAbilityBlueprintLibrary::SetComponentTickEnableImplicitRef(TEXT("DynamicSetAnimationTickable"), SkeletalMeshComponent, Context);
			}
		}
	}
	if (m_EnableAnimMove)
	{
		AActor* Actor = Context->GetSelfActor();
		if (IsValid(Actor))
		{
			USPCharacterMovementComponent* MovementComponent = Actor->FindComponentByClass<
				USPCharacterMovementComponent>();
			ASPGameMonsterBase* Monster = Cast<ASPGameMonsterBase>(Context->GetOwner());

			if (IsValid(MovementComponent))
			{
				if (IsValid(Monster))
				{
					MovementComponent->SetShouldAnimMove(true, FVector::ZeroVector, m_CloseFriction, Monster->IsCommonSkillAnimMoveByZMap.Contains(Stage) ? Monster->IsCommonSkillAnimMoveByZMap[Stage] : false, m_OnlyXYMoveByLoc);
				}
				else
				{
					MovementComponent->SetShouldAnimMove(true, FVector::ZeroVector, m_CloseFriction, false, m_OnlyXYMoveByLoc);
				}
			}

			const FString StageSequenceName = GetSequenceName(MonsterAnimInstance, GetStagePropertyName(Stage));
			if (!StageSequenceName.IsEmpty())
			{
				MovementComponent->StartPlayAnimMovement(StageSequenceName);
			}
			else
			{
				if (IsValid(Monster))
				{
					int32 MonsterID = Monster->MonsterID;
					if (AnimMoveSequenceMap.Contains(MonsterID))
					{
						if (m_DirectLoadXml && IsValid(AnimMoveSequenceMap[MonsterID].LoadSynchronous()))
						{
							MovementComponent->StartPlayAnimMovement(AnimMoveSequenceMap[MonsterID]->GetName());
						}
					}
				}
			}
		}
	}
}

FString USPSeAbilityStageTask::GetStagePropertyName(const ESPSkillStage Stage)
{
	switch (Stage)
	{
	case ESPSkillStage::Start:
		return "A_FarSkill_Start";
	case ESPSkillStage::StartLoop:
		return "A_FarSkill_StartLoop";
	case ESPSkillStage::Action:
		return "A_FarSkill_Action";
	case ESPSkillStage::ActionLoop:
		return "A_FarSkill_ActionLoop";
	case ESPSkillStage::End:
		return "A_FarSkill_End";
	default:
		return "";
	}
}

float USPSeAbilityStageTask::GetSequencePlayLength(const USPMonsterAnimInstance* AnimInstance,
                                                   const FString& PropertyName)
{
	if (AnimInstance == nullptr || PropertyName.IsEmpty())
		return 0;
	
	FObjectProperty* AnimSequenceProperty = static_cast<FObjectProperty*>(
		FindFProperty<FProperty>(AnimInstance->GetClass(), *PropertyName));
	if (AnimSequenceProperty == nullptr)
		return 0;

	UAnimSequenceBase* TargetSequence = Cast<UAnimSequenceBase>(
		AnimSequenceProperty->GetObjectPropertyValue_InContainer(AnimInstance));
	if (TargetSequence == nullptr)
		return 0;

	return TargetSequence->GetPlayLength();
}

FString USPSeAbilityStageTask::GetSequenceName(const USPMonsterAnimInstance* AnimInstance, const FString& PropertyName)
{
	if (AnimInstance == nullptr || PropertyName.IsEmpty())
		return TEXT("");
	
	FObjectProperty* AnimSequenceProperty = static_cast<FObjectProperty*>(
		FindFProperty<FProperty>(AnimInstance->GetClass(), *PropertyName));
	if (AnimSequenceProperty == nullptr)
		return TEXT("");

	UAnimSequenceBase* TargetSequence = Cast<UAnimSequenceBase>(
		AnimSequenceProperty->GetObjectPropertyValue_InContainer(AnimInstance));
	if (TargetSequence == nullptr)
		return TEXT("");

	return TargetSequence->GetName();
}

UAbleAbilityTaskScratchPad* USPSeAbilityStageTask::CreateScratchPad(
	const TWeakObjectPtr<UAbleAbilityContext>& Context) const
{
	if (UAbleAbilityUtilitySubsystem* Subsystem = Context->GetUtilitySubsystem())
	{
		static TSubclassOf<UAbleAbilityTaskScratchPad> ScratchPadClass =
			USPSetAbilityStageScratchPad::StaticClass();
		return Subsystem->FindOrConstructTaskScratchPad(ScratchPadClass);
	}

	return NewObject<USPSetAbilityStageScratchPad>(Context.Get());
}

void USPSeAbilityStageTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
                                      const EAbleAbilityTaskResult result) const
{
	Super::OnTaskEnd(Context, result);
	OnTaskEndBP(Context.Get(), result);
}

void USPSeAbilityStageTask::OnTaskEndBP_Implementation(const UAbleAbilityContext* Context,
                                                       const EAbleAbilityTaskResult result) const
{
	if (m_EnableAnimMove)
	{
		AActor* Actor = Context->GetSelfActor();
		if (IsValid(Actor))
		{
			USPCharacterMovementComponent* MovementComponent = Actor->FindComponentByClass<
				USPCharacterMovementComponent>();
			if (IsValid(MovementComponent))
			{
				MovementComponent->SetShouldAnimMove(false, FVector::ZeroVector, m_CloseFriction);
			}
		}
	}
	
	// todo...目前没找到在技能结束时统一写后处理地方，部分回退逻辑无法很好的实现，以及判断分段式task在最后做收尾处理
	// todo...先临时手配一个字段来进行标识 @bladeyuan
	if (Last || result == Interrupted)
	{
		const AActor* Target = Context->GetOwner();
		if (!IsValid(Target)) return;

		const USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(Target->GetComponentByClass(
			USkeletalMeshComponent::StaticClass()));

		if (!SkeletalMeshComponent) return;

		UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance();
		if (!AnimInstance || !AnimInstance->IsA(USPMonsterAnimInstance::StaticClass())) return;
		USPMonsterAnimInstance* MonsterAnimInstance = Cast<USPMonsterAnimInstance>(AnimInstance);
		if (MonsterAnimInstance)
		{
			MonsterAnimInstance->SetSkillType(ESPSkillType::Normal);
			UE_LOG(LogTemp, Warning, TEXT("USPSeAbilityStageTask::OnTaskEndBP_Implementation Name:%s"), *Target->GetName());
		}
	}
	USPSetAbilityStageScratchPad* ScratchPad = Cast<USPSetAbilityStageScratchPad>(
	Context->GetScratchPadForTask(this));
	if (!ScratchPad) return;
	if (m_OverrideVisibilityBasedAnimTick && !ScratchPad->CachedVisibilityBasedAnimTickOptionMap.IsEmpty())
	{
		for (auto MeshIt = ScratchPad->CachedVisibilityBasedAnimTickOptionMap.CreateIterator(); MeshIt; ++MeshIt)
		{
			if (MeshIt.Key().IsValid())
			{
				MeshIt.Key()->VisibilityBasedAnimTickOption = MeshIt.Value();
				if (UKismetSystemLibrary::IsDedicatedServer(this))
				{
					UAbleAbilityBlueprintLibrary::SetComponentTickDisableImplicitRef(TEXT("DynamicSetAnimationTickable"), MeshIt.Key().Get(), Context);
				}
			}
		}
	}
}

bool USPSeAbilityStageTask::IsDone(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	return IsDoneBP(Context.Get());
}

bool USPSeAbilityStageTask::IsDoneBP_Implementation(const UAbleAbilityContext* Context) const
{
	if (!m_Adaption) return Super::IsDoneActually(Context);
	
	const USPSetAbilityStageScratchPad* ScratchPad = Cast<USPSetAbilityStageScratchPad>(
		Context->GetScratchPadForTask(this));
	if (!ScratchPad) return true;

	if (!IsValid(ScratchPad->AnimInstanceCache)) return true;

	const float CurrentTime = Context->GetCurrentTime();
	const float StartTime = GetActualStartTime();
	const float StageSequenceLength = GetSequencePlayLength(ScratchPad->AnimInstanceCache, GetStagePropertyName(Stage));
	const bool bDone = CurrentTime - StartTime >= StageSequenceLength;
	// UE_LOG(LogTemp, Warning, TEXT("[%s] Task IsDone = %d, [%f] - [%f] >= [%f]"), *GetNameSafe(this), bDone, CurrentTime, StartTime, StageSequenceLength);
	return bDone;
}

TStatId USPSeAbilityStageTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPSeAbilityStageTask, STATGROUP_USPAbility);
}

bool USPSeAbilityStageTask::IsSingleFrameBP_Implementation() const
{
	return false;
}

EAbleAbilityTaskRealm USPSeAbilityStageTask::GetTaskRealmBP_Implementation() const
{
	return EAbleAbilityTaskRealm::ATR_ClientAndServer;
}

#undef LOCTEXT_NAMESPACE

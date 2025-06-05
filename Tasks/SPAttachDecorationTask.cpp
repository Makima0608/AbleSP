// Copyright (c) Extra Life Studios, LLC. All rights reserved.


#include "Game/SPGame/Skill/Task/SPAttachDecorationTask.h"

#include "ableSubSystem.h"
#include "MoeGameLog.h"
#include "MoeFeatureSPLog.h"
#include "Game/SPGame/Monster/SPGameCharInventoryComponent.h"
#include "Game/SPGame/Skill/Core/SPAbilityFunctionLibrary.h"
#include "Game/SPGame/Utils/SPGameLibrary.h"
#include "MoeGameplay/Character/Component/MoeCharInventoryComponent.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

USAttachDecoratorTaskScratchPad::USAttachDecoratorTaskScratchPad()
{
}

USAttachDecoratorTaskScratchPad::~USAttachDecoratorTaskScratchPad()
{
}

USPAttachDecorationTask::USPAttachDecorationTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), m_Target(EAbleAbilityTargetType::ATT_Self),
	  m_AttachRule(EDecorationAttachRule::StaticMesh)
{
}

USPAttachDecorationTask::~USPAttachDecorationTask()
{
}

FString USPAttachDecorationTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPAttachDecorationTask");
}

void USPAttachDecorationTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);
	OnTaskStartBP(Context.Get());
}

void USPAttachDecorationTask::OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const
{
	USAttachDecoratorTaskScratchPad* ScratchPad = Cast<USAttachDecoratorTaskScratchPad>(
		Context->GetScratchPadForTask(this));
	if (!ScratchPad) return;

	AActor* AttachActor = GetSingleActorFromTargetType(Context, m_Target);
	if (!IsValid(AttachActor)) return;
	
	USkeletalMeshComponent* SkeletonMeshComponent = Cast<USkeletalMeshComponent>(
		AttachActor->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	if (!IsValid(SkeletonMeshComponent)) return;

	UWorld* CurrentWorld = Context->GetWorld();
	if (!CurrentWorld)
	{
		return;
	}
	
	AActor* AnimationActor = CurrentWorld->SpawnActor<AActor>(AActor::StaticClass());
	if (!AnimationActor)
	{
		return;
	}

	USceneComponent* RootComponent = NewObject<USceneComponent>(AnimationActor, TEXT("DefaultSceneRoot"));
	AnimationActor->SetRootComponent(RootComponent);
	AnimationActor->AddInstanceComponent(RootComponent);
	RootComponent->RegisterComponent();
	
	UMeshComponent* DecorationMeshComponent = CreateMeshComponentToAttach(AnimationActor, SkeletonMeshComponent);
	if (!IsValid(DecorationMeshComponent))
	{
		MOE_SP_ABILITY_ERROR(
			TEXT("[SPAbility] SPAttachDecorationTask attach failed, decoration component null after switch !"));
		return;
	}
	
	AnimationActor->AttachToComponent(SkeletonMeshComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, m_Socket);

	DecorationMeshComponent->MobileStencilSetting = EMobileStencilSetting::EMSV_NoCut;
	DecorationMeshComponent->SetCastShadow(false);
	DecorationMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DecorationMeshComponent->AttachToComponent(RootComponent,
	                                           FAttachmentTransformRules::KeepRelativeTransform);
	DecorationMeshComponent->SetRelativeTransform(m_DecorationRelativeTransform);
	ScratchPad->DecorationComponent = DecorationMeshComponent;
	ScratchPad->DecorationActor = AnimationActor;

	if (m_AttachRule == EDecorationAttachRule::SkeletalMesh && !m_AnimationMontageOnstart.IsNull())
	{
		if (USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(DecorationMeshComponent))
		{
			SkeletalMeshComponent->PlayAnimation(m_AnimationMontageOnstart.LoadSynchronous(), bIsLoop);
		}
	}

	// hide weapon
	USPGameCharInventoryComponent* InventoryComponent = Cast<USPGameCharInventoryComponent>(
		AttachActor->GetComponentByClass(USPGameCharInventoryComponent::StaticClass()));
	if (IsValid(InventoryComponent))
		InventoryComponent->SPSetWeaponHideInGame(true);
}

void USPAttachDecorationTask::OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
                                         float deltaTime) const
{
	Super::OnTaskTick(Context, deltaTime);
	OnTaskTickBP(Context.Get(), deltaTime);
}

void USPAttachDecorationTask::OnTaskTickBP_Implementation(const UAbleAbilityContext* Context, float deltaTime) const
{

#if WITH_EDITOR
	const USAttachDecoratorTaskScratchPad* ScratchPad = Cast<USAttachDecoratorTaskScratchPad>(
		Context->GetScratchPadForTask(this));
	if (!ScratchPad) return;

	if (IsValid(ScratchPad->DecorationComponent))
		ScratchPad->DecorationComponent->SetRelativeTransform(m_DecorationRelativeTransform);

#endif
}

void USPAttachDecorationTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
                                        const EAbleAbilityTaskResult result) const
{
	Super::OnTaskEnd(Context, result);
	OnTaskEndBP(Context.Get(), result);
}

void USPAttachDecorationTask::OnTaskEndBP_Implementation(const UAbleAbilityContext* Context,
                                                         const EAbleAbilityTaskResult result) const
{
	USAttachDecoratorTaskScratchPad* ScratchPad = Cast<USAttachDecoratorTaskScratchPad>(
		Context->GetScratchPadForTask(this));
	if (!ScratchPad) return;

	if (IsValid(ScratchPad->DecorationActor))
	{
		ScratchPad->DecorationComponent->DestroyComponent();
		ScratchPad->DecorationComponent = nullptr;
		ScratchPad->DecorationActor->Destroy();
		ScratchPad->DecorationActor = nullptr;
	}
	else
	{
		MOE_SP_ABILITY_WARN(
			TEXT(
				"[SPAbility] USPAttachDecorationTask::OnTaskEnd Warning ! Destroy Decoration Mesh failed because component invalid !"
			))
	}

	// hide weapon
	const AActor* AttachActor = GetSingleActorFromTargetType(Context, m_Target);
	if (!IsValid(AttachActor)) return;

	USPGameCharInventoryComponent* InventoryComponent = Cast<USPGameCharInventoryComponent>(
		AttachActor->GetComponentByClass(USPGameCharInventoryComponent::StaticClass()));
	if (IsValid(InventoryComponent))
		InventoryComponent->SPSetWeaponHideInGame(false);
}

UMeshComponent* USPAttachDecorationTask::CreateMeshComponentToAttach(AActor* AttachActor, const USkeletalMeshComponent* SkeletonMeshComponent) const
{
	const FTransform RelativeTrans = FTransform(SkeletonMeshComponent->GetRelativeTransform());
	UMeshComponent* DecorationMeshComponent = nullptr;
	
	switch (m_AttachRule)
	{
	case EDecorationAttachRule::StaticMesh:
		{
			UStaticMeshComponent* MeshComponent = Cast<UStaticMeshComponent>(
				AttachActor->AddComponentByClass(
					UStaticMeshComponent::StaticClass(), true, RelativeTrans, false));
			MeshComponent->SetStaticMesh(m_DecorationMesh.LoadSynchronous());
			DecorationMeshComponent = MeshComponent;
		}
		break;
	case EDecorationAttachRule::SkeletalMesh:
		{
			USkeletalMeshComponent* MeshComponent = Cast<USkeletalMeshComponent>(
				AttachActor->AddComponentByClass(
					USkeletalMeshComponent::StaticClass(), true, RelativeTrans, false));
			MeshComponent->SetSkeletalMesh(m_DecorationSkeletalMesh.LoadSynchronous());
			DecorationMeshComponent = MeshComponent;
		}
		break;
	default:
		break;
	}

	return DecorationMeshComponent;
}

UAbleAbilityTaskScratchPad* USPAttachDecorationTask::CreateScratchPad(
	const TWeakObjectPtr<UAbleAbilityContext>& Context) const
{
	if (UAbleAbilityUtilitySubsystem* Subsystem = Context->GetUtilitySubsystem())
	{
		static TSubclassOf<UAbleAbilityTaskScratchPad> ScratchPadClass =
			USAttachDecoratorTaskScratchPad::StaticClass();
		return Subsystem->FindOrConstructTaskScratchPad(ScratchPadClass);
	}

	return NewObject<USAttachDecoratorTaskScratchPad>(Context.Get());
}

TStatId USPAttachDecorationTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPAttachDecorationTask, STATGROUP_USPAbility);
}

bool USPAttachDecorationTask::IsSingleFrameBP_Implementation() const { return false; }

#undef LOCTEXT_NAMESPACE

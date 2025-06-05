// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SPGame/Skill/Task/SPAttachMeshTask.h"

#include "ableSubSystem.h"
#include "MoeFeatureSPLog.h"
#include "Game/SPGame/Utils/SPGameLibrary.h"

USPAttachMeshTaskScratchPad::USPAttachMeshTaskScratchPad()
{
}

USPAttachMeshTaskScratchPad::~USPAttachMeshTaskScratchPad()
{
}

FString USPAttachMeshTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPAttachMeshTask");
}

void USPAttachMeshTask::ResetScratchPad(UAbleAbilityTaskScratchPad* ScratchPad) const
{
	Super::ResetScratchPad(ScratchPad);
	if (USPAttachMeshTaskScratchPad* CustomScratchPad = Cast<USPAttachMeshTaskScratchPad>(ScratchPad))
	{
		CustomScratchPad->AttachComponent = nullptr;	
	}
}

void USPAttachMeshTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);
	USPAttachMeshTaskScratchPad* ScratchPad = Cast<USPAttachMeshTaskScratchPad>(
		Context->GetScratchPadForTask(this));
	if (!ScratchPad) return;
	if (!m_IsDeAttach)
	{
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

		UMeshComponent* AttachMeshComponent = CreateMeshComponentToAttach(AttachActor, SkeletonMeshComponent);
		if (!IsValid(AttachMeshComponent))
		{
			MOE_SP_ABILITY_ERROR(
				TEXT("[SPAbility] SPAttachMeshTask attach failed, decoration component null after switch !"));
			return;
		}

		AttachMeshComponent->MobileStencilSetting = EMobileStencilSetting::EMSV_NoCut;
		AttachMeshComponent->SetCastShadow(false);
		AttachMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AttachMeshComponent->AttachToComponent(SkeletonMeshComponent, FAttachmentTransformRules::KeepRelativeTransform,
		                                       m_Socket);
		AttachMeshComponent->SetRelativeTransform(m_DecorationRelativeTransform);
		ScratchPad->AttachComponent = AttachMeshComponent;
	}
	else
	{
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

		// 获取所有附加到 SkeletalMeshComponent 的子组件
		TArray<USceneComponent*> AttachedComponents;
		SkeletonMeshComponent->GetChildrenComponents(true, AttachedComponents);

		for (USceneComponent* Component : AttachedComponents)
		{
			UMeshComponent* MeshComp = Cast<UMeshComponent>(Component);
			if (IsValid(MeshComp))
			{
				if (MeshComp && MeshComp->GetAttachSocketName() == m_Socket)
				{
					// 分离组件
					MeshComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

					if (!m_IsRecoverOnEnd)
					{
						// 销毁组件
						Component->DestroyComponent();
					}
					else
					{
						ScratchPad->AttachComponent = MeshComp;
						MeshComp->SetWorldLocation(FVector(0, 0, 0));
					}
				}
			}
		}
	}
	return;
	// OnTaskStartBP(Context.Get());
}

void USPAttachMeshTask::OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const
{
	USPAttachMeshTaskScratchPad* ScratchPad = Cast<USPAttachMeshTaskScratchPad>(
	Context->GetScratchPadForTask(this));
	if (!ScratchPad) return;
	if (!m_IsDeAttach)
	{
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

		UMeshComponent* AttachMeshComponent = CreateMeshComponentToAttach(AttachActor, SkeletonMeshComponent);
		if (!IsValid(AttachMeshComponent))
		{
			MOE_SP_ABILITY_ERROR(
				TEXT("[SPAbility] SPAttachMeshTask attach failed, decoration component null after switch !"));
			return;
		}

		AttachMeshComponent->MobileStencilSetting = EMobileStencilSetting::EMSV_NoCut;
		AttachMeshComponent->SetCastShadow(false);
		AttachMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AttachMeshComponent->AttachToComponent(SkeletonMeshComponent, FAttachmentTransformRules::KeepRelativeTransform,
		                                       m_Socket);
		AttachMeshComponent->SetRelativeTransform(m_DecorationRelativeTransform);
		ScratchPad->AttachComponent = AttachMeshComponent;
	}
	else
	{
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

		// 获取所有附加到 SkeletalMeshComponent 的子组件
		TArray<USceneComponent*> AttachedComponents;
		SkeletonMeshComponent->GetChildrenComponents(true, AttachedComponents);

		for (USceneComponent* Component : AttachedComponents)
		{
			UMeshComponent* MeshComp = Cast<UMeshComponent>(Component);
			if (IsValid(MeshComp))
			{
				if (MeshComp && MeshComp->GetAttachSocketName() == m_Socket)
				{
					// 分离组件
					MeshComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

					if (!m_IsRecoverOnEnd)
					{
						// 销毁组件
						Component->DestroyComponent();
					}
					else
					{
						ScratchPad->AttachComponent = MeshComp;
						MeshComp->SetWorldLocation(FVector(0, 0, 0));
					}
				}
			}
		}
	}
}

void USPAttachMeshTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
                                  const EAbleAbilityTaskResult result) const
{
	Super::OnTaskEnd(Context, result);
	USPAttachMeshTaskScratchPad* ScratchPad = Cast<USPAttachMeshTaskScratchPad>(
	Context->GetScratchPadForTask(this));
	if (!ScratchPad) return;

	if (!m_IsDeAttach)
	{
		if (m_IsRecoverOnEnd)
		{
			if (IsValid(ScratchPad->AttachComponent))
			{
				ScratchPad->AttachComponent->DestroyComponent();
				ScratchPad->AttachComponent = nullptr;
			}
			else
			{
				MOE_SP_ABILITY_WARN(
					TEXT(
						"[SPAbility] USPAttachDecorationTask::OnTaskEnd Warning ! Destroy Decoration Mesh failed because component invalid !"
					))
			}
		}
	}
	else
	{
		if (m_IsRecoverOnEnd)
		{
			if (IsValid(ScratchPad->AttachComponent))
			{
				AActor* AttachActor = GetSingleActorFromTargetType(Context, m_Target);
				if (!IsValid(AttachActor)) return;

				USkeletalMeshComponent* SkeletonMeshComponent = Cast<USkeletalMeshComponent>(
					AttachActor->GetComponentByClass(USkeletalMeshComponent::StaticClass()));

				if (!IsValid(SkeletonMeshComponent)) return;

				ScratchPad->AttachComponent->MobileStencilSetting = EMobileStencilSetting::EMSV_NoCut;
				ScratchPad->AttachComponent->SetCastShadow(false);
				ScratchPad->AttachComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				ScratchPad->AttachComponent->AttachToComponent(SkeletonMeshComponent,
															   FAttachmentTransformRules::KeepRelativeTransform,
															   m_Socket);
				ScratchPad->AttachComponent->SetRelativeTransform(m_DecorationRelativeTransform);
			}
			else
			{
				MOE_SP_ABILITY_WARN(
					TEXT(
						"[SPAbility] USPAttachDecorationTask::OnTaskEnd Warning ! recover Mesh failed because component invalid !"
					))
			}
		}
	}
	return;
	// OnTaskEndBP(Context.Get(), result);
}

void USPAttachMeshTask::OnTaskEndBP_Implementation(const UAbleAbilityContext* Context,
	const EAbleAbilityTaskResult result) const
{
	USPAttachMeshTaskScratchPad* ScratchPad = Cast<USPAttachMeshTaskScratchPad>(
		Context->GetScratchPadForTask(this));
	if (!ScratchPad) return;

	if (!m_IsDeAttach)
	{
		if (m_IsRecoverOnEnd)
		{
			if (IsValid(ScratchPad->AttachComponent))
			{
				ScratchPad->AttachComponent->DestroyComponent();
				ScratchPad->AttachComponent = nullptr;
			}
			else
			{
				MOE_SP_ABILITY_WARN(
					TEXT(
						"[SPAbility] USPAttachDecorationTask::OnTaskEnd Warning ! Destroy Decoration Mesh failed because component invalid !"
					))
			}
		}
	}
	else
	{
		if (m_IsRecoverOnEnd)
		{
			if (IsValid(ScratchPad->AttachComponent))
			{
				AActor* AttachActor = GetSingleActorFromTargetType(Context, m_Target);
				if (!IsValid(AttachActor)) return;

				USkeletalMeshComponent* SkeletonMeshComponent = Cast<USkeletalMeshComponent>(
					AttachActor->GetComponentByClass(USkeletalMeshComponent::StaticClass()));

				if (!IsValid(SkeletonMeshComponent)) return;

				ScratchPad->AttachComponent->MobileStencilSetting = EMobileStencilSetting::EMSV_NoCut;
				ScratchPad->AttachComponent->SetCastShadow(false);
				ScratchPad->AttachComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				ScratchPad->AttachComponent->AttachToComponent(SkeletonMeshComponent,
				                                               FAttachmentTransformRules::KeepRelativeTransform,
				                                               m_Socket);
				ScratchPad->AttachComponent->SetRelativeTransform(m_DecorationRelativeTransform);
			}
			else
			{
				MOE_SP_ABILITY_WARN(
					TEXT(
						"[SPAbility] USPAttachDecorationTask::OnTaskEnd Warning ! recover Mesh failed because component invalid !"
					))
			}
		}
	}

}

UMeshComponent* USPAttachMeshTask::CreateMeshComponentToAttach(AActor* AttachActor,
	const USkeletalMeshComponent* SkeletonMeshComponent) const
{
	const FTransform RelativeTrans = FTransform(SkeletonMeshComponent->GetRelativeTransform());
	UMeshComponent* AttachMeshComponent = nullptr;
	
	switch (m_AttachRule)
	{
	case EMeshAttachRule::AttachStaticMesh:
		{
			UStaticMeshComponent* MeshComponent = Cast<UStaticMeshComponent>(
				AttachActor->AddComponentByClass(
					UStaticMeshComponent::StaticClass(), true, RelativeTrans, false));
			MeshComponent->SetStaticMesh(m_DecorationMesh.LoadSynchronous());
			AttachMeshComponent = MeshComponent;
		}
		break;
	case EMeshAttachRule::AttachSkeletalMesh:
		{
			USkeletalMeshComponent* MeshComponent = Cast<USkeletalMeshComponent>(
				AttachActor->AddComponentByClass(
					USkeletalMeshComponent::StaticClass(), true, RelativeTrans, false));
			MeshComponent->SetSkeletalMesh(m_DecorationSkeletalMesh.LoadSynchronous());
			AttachMeshComponent = MeshComponent;
		}
		break;
	default:
		break;
	}

	return AttachMeshComponent;
}

UAbleAbilityTaskScratchPad* USPAttachMeshTask::CreateScratchPad(
	const TWeakObjectPtr<UAbleAbilityContext>& Context) const
{
	if (UAbleAbilityUtilitySubsystem* Subsystem = Context->GetUtilitySubsystem())
	{
		static TSubclassOf<UAbleAbilityTaskScratchPad> ScratchPadClass =
			USPAttachMeshTaskScratchPad::StaticClass();
		return Subsystem->FindOrConstructTaskScratchPad(ScratchPadClass);
	}

	return NewObject<USPAttachMeshTaskScratchPad>(Context.Get());
}

TStatId USPAttachMeshTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPAttachMeshTask, STATGROUP_USPAbility);
}

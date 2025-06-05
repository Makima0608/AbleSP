// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SPGame/Skill/Task/SPSetBoneCapsuleTask.h"

#include "Game/SPGame/Character/SPGameMonsterBase.h"
#include "Game/SPGame/Skill/Task/SPSetBoneCapsuleTaskScratchPad.h"

TStatId USPSetBoneCapsuleTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPSetBoneCapsuleTask, STATGROUP_USPAbility);
}

void USPSetBoneCapsuleTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);

	AActor* Actor = Context->GetSelfActor();
	if (IsValid(Actor))
	{
		ASPGameMonsterBase* Monster = Cast<ASPGameMonsterBase>(Actor);

		if (!IsValid(Monster))
		{
			return;
		}

		USkeletalMeshComponent* MeshComp = Monster->GetSkeletalMeshComponent();
		UPhysicsAsset* PhysicsAsset = MeshComp->GetPhysicsAsset();
		if (IsValid(MeshComp))
		{
			for (const FName& BoneName : BonesName)
			{
				if (bIsHideBone)
				{
					// Monster->HideBoneRecursive(MeshComp, BoneName);
					MeshComp->HideBoneByName(BoneName, EPhysBodyOp::PBO_None);
					
					if (IsValid(PhysicsAsset))
					{
						for (int32 Index = 0; Index < PhysicsAsset->SkeletalBodySetups.Num(); Index++)
						{
							UBodySetup* BodySetup = PhysicsAsset->SkeletalBodySetups[Index];
							if (BodySetup && BodySetup->BoneName == BoneName)
							{
								BodySetup->CollisionReponse = EBodyCollisionResponse::BodyCollision_Disabled;

								BodySetup->MarkPackageDirty();
								MeshComp->RecreatePhysicsState();
							}
						}
					}
				}
				else
				{
					MeshComp->UnHideBoneByName(BoneName);
					
					if (IsValid(PhysicsAsset))
					{
						for (int32 Index = 0; Index < PhysicsAsset->SkeletalBodySetups.Num(); Index++)
						{
							UBodySetup* BodySetup = PhysicsAsset->SkeletalBodySetups[Index];
							if (BodySetup && BodySetup->BoneName == BoneName)
							{
								BodySetup->CollisionReponse = EBodyCollisionResponse::BodyCollision_Enabled;

								BodySetup->MarkPackageDirty();
								MeshComp->RecreatePhysicsState();
							}
						}
					}
				}
			}
		}
	}
}

void USPSetBoneCapsuleTask::OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context, float deltaTime) const
{
	Super::OnTaskTick(Context, deltaTime);
}

void USPSetBoneCapsuleTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	const EAbleAbilityTaskResult result) const
{
	Super::OnTaskEnd(Context, result);

	if (bIsRecoverWhenEnd)
	{
		USPSetBoneCapsuleTaskScratchPad* ScratchPad = Cast<USPSetBoneCapsuleTaskScratchPad>(
			Context->GetScratchPadForTask(this));
		if (!ScratchPad) return;
		FVector PreScale = ScratchPad->PreScale;
		AActor* Actor = Context->GetSelfActor();
		if (IsValid(Actor))
		{
			ASPGameMonsterBase* Monster = Cast<ASPGameMonsterBase>(Actor);

			if (!IsValid(Monster))
			{
				return;
			}

			USkeletalMeshComponent* MeshComp = Monster->GetSkeletalMeshComponent();
			UPhysicsAsset* PhysicsAsset = MeshComp->GetPhysicsAsset();
			if (IsValid(MeshComp))
			{
				for (const FName& BoneName : BonesName)
				{
					if (bIsHideBone)
					{
						MeshComp->UnHideBoneByName(BoneName);

						if (IsValid(PhysicsAsset))
						{
							for (int32 Index = 0; Index < PhysicsAsset->SkeletalBodySetups.Num(); Index++)
							{
								UBodySetup* BodySetup = PhysicsAsset->SkeletalBodySetups[Index];
								if (BodySetup && BodySetup->BoneName == BoneName)
								{
									BodySetup->CollisionReponse = EBodyCollisionResponse::BodyCollision_Enabled;

									BodySetup->MarkPackageDirty();
									MeshComp->RecreatePhysicsState();
								}
							}
						}
					}
					else
					{
						MeshComp->HideBoneByName(BoneName, EPhysBodyOp::PBO_None);
					
						if (IsValid(PhysicsAsset))
						{
							for (int32 Index = 0; Index < PhysicsAsset->SkeletalBodySetups.Num(); Index++)
							{
								UBodySetup* BodySetup = PhysicsAsset->SkeletalBodySetups[Index];
								if (BodySetup && BodySetup->BoneName == BoneName)
								{
									BodySetup->CollisionReponse = EBodyCollisionResponse::BodyCollision_Disabled;

									BodySetup->MarkPackageDirty();
									MeshComp->RecreatePhysicsState();
								}
							}
						}
					}
				}
			}
		}
	}
}

EAbleAbilityTaskRealm USPSetBoneCapsuleTask::GetTaskRealmBP_Implementation() const
{
	return EAbleAbilityTaskRealm::ATR_ClientAndServer;
}

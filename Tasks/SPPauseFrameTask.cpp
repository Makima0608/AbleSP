// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SPGame/Skill/Task/SPPauseFrameTask.h"

#include "ableSubSystem.h"
#include "MoeFeatureSPLog.h"
#include "Game/SPGame/Character/SPGameCharacterBase.h"

USPPauseFrameTaskScratchPad::USPPauseFrameTaskScratchPad()
{
}

USPPauseFrameTaskScratchPad::~USPPauseFrameTaskScratchPad()
{
}

void USPPauseFrameTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);
	OnTaskStartBP(Context.Get());
}

void USPPauseFrameTask::OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const
{
	MOE_SP_ABILITY_LOG(TEXT("USPPauseFrameTask::OnTaskStartBP"));

	USPPauseFrameTaskScratchPad* ScratchPad = Cast<USPPauseFrameTaskScratchPad>(
	Context->GetScratchPadForTask(this));
	if (!ScratchPad) return;
	ScratchPad->DuringTime = 0.0f;
}

void USPPauseFrameTask::OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context, float deltaTime) const
{
	Super::OnTaskTick(Context, deltaTime);
	OnTaskTickBP(const_cast<UAbleAbilityContext*>(Context.Get()), deltaTime);
}

void USPPauseFrameTask::OnTaskTickBP_Implementation(const UAbleAbilityContext* Context, float deltaTime) const
{
	USPPauseFrameTaskScratchPad* ScratchPad = Cast<USPPauseFrameTaskScratchPad>(
		Context->GetScratchPadForTask(this));
	if (!ScratchPad) return;
	ScratchPad->DuringTime += deltaTime;

	if (!ScratchPad->bIsDuringPause)
	{
		if (StartPauseTimeList.Num() > 0)
		{
			for (int i = ScratchPad->HasPauseIndex; i < StartPauseTimeList.Num(); i++)
			{
				if (ScratchPad->DuringTime > StartPauseTimeList[i])
				{
					// if (i == StartPauseTimeList.Num() - 1 || ScratchPad->DuringTime < StartPauseTimeList[i + 1])
					// {
						AActor* Actor = Context->GetSelfActor();
						ASPGameCharacterBase* SPChar = Cast<ASPGameCharacterBase>(Actor);
						if (IsValid(SPChar))
						{
							USkeletalMeshComponent* MeshComp = SPChar->GetMesh();
							if (IsValid(MeshComp))
							{
								UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
								if (IsValid(AnimInstance))
								{
									UAnimMontage* CurrentMontage = AnimInstance->GetCurrentActiveMontage();
									if (IsValid(CurrentMontage))
									{
										float MontagePauseTime = AnimInstance->Montage_GetPosition(CurrentMontage);
										ScratchPad->MontagePauseTime = MontagePauseTime;

										AnimInstance->Montage_Pause(CurrentMontage);
										ScratchPad->PauseStartTime = ScratchPad->DuringTime;
										ScratchPad->bIsDuringPause = true;
										ScratchPad->HasPauseIndex++;

										if (bIsOpenDebugPoint)
										{
											DrawDebugPoint(GetWorld(), SPChar->GetActorLocation() + FVector(0, 0, 240), 25.0f, FColor::Yellow, false, 0.15f);
										}
									}
								}
							}
						}
					// }
				}
			}
		}
	}
	else
	{
		if (ScratchPad->DuringTime > ScratchPad->PauseStartTime + PauseDuringTime)
		{
			AActor* Actor = Context->GetSelfActor();
			ASPGameCharacterBase* SPChar = Cast<ASPGameCharacterBase>(Actor);
			if (IsValid(SPChar))
			{
				USkeletalMeshComponent* MeshComp = SPChar->GetMesh();
				if (IsValid(MeshComp))
				{
					UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
					if (IsValid(AnimInstance))
					{
						UAnimMontage* CurrentMontage = AnimInstance->GetCurrentActiveMontage();
						if (IsValid(CurrentMontage))
						{
							AnimInstance->Montage_Resume(CurrentMontage);
							AnimInstance->Montage_SetPosition(CurrentMontage, bIsJumpOver ? ScratchPad->MontagePauseTime + PauseDuringTime : ScratchPad->MontagePauseTime);

							ScratchPad->bIsDuringPause = false;
						}
					}
				}
			}
		}
	}
}

void USPPauseFrameTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	const EAbleAbilityTaskResult result) const
{
	OnTaskEndBP(const_cast<UAbleAbilityContext*>(Context.Get()), result);
	Super::OnTaskEnd(Context, result);
}

void USPPauseFrameTask::OnTaskEndBP_Implementation(const UAbleAbilityContext* Context,
												   const EAbleAbilityTaskResult result) const
{
	USPPauseFrameTaskScratchPad* ScratchPad = Cast<USPPauseFrameTaskScratchPad>(
	Context->GetScratchPadForTask(this));
	if (!ScratchPad) return;
	ScratchPad->DuringTime = 0.0f;
	ScratchPad->bIsDuringPause = false;
	ScratchPad->MontagePauseTime = 0.0f;
	ScratchPad->PauseStartTime = 0.0f;
	ScratchPad->HasPauseIndex = 0;
}

TStatId USPPauseFrameTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPPauseFrameTask, STATGROUP_USPAbility);
}

EAbleAbilityTaskRealm USPPauseFrameTask::GetTaskRealm() const
{
	return EAbleAbilityTaskRealm::ATR_ClientAndServer;
}

UAbleAbilityTaskScratchPad* USPPauseFrameTask::CreateScratchPad(
	const TWeakObjectPtr<UAbleAbilityContext>& Context) const
{
	if (UAbleAbilityUtilitySubsystem* Subsystem = Context->GetUtilitySubsystem())
	{
		static TSubclassOf<UAbleAbilityTaskScratchPad> ScratchPadClass =
			USPPauseFrameTaskScratchPad::StaticClass();
		return Subsystem->FindOrConstructTaskScratchPad(ScratchPadClass);
	}

	return NewObject<USPPauseFrameTaskScratchPad>(Context.Get());
}







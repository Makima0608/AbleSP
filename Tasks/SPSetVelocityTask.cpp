// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SPGame/Skill/Task/SPSetVelocityTask.h"

#include "ableSubSystem.h"
#include "Game/SPGame/Character/SPCharacterMovementComponent.h"
#include "Game/SPGame/Character/SPGameMonsterBase.h"
#include "Game/SPGame/Skill/Task/SPSetVelocityTaskScratchPad.h"
#include "Game/SPGame/Utils/SPGameLibrary.h"

void USPSetVelocityTask::MoveActorByVelocity(const TWeakObjectPtr<const UAbleAbilityContext>& Context, float deltaTime) const
{
	USPSetVelocityTaskScratchPad* ScratchPad = Cast<USPSetVelocityTaskScratchPad>(
	Context->GetScratchPadForTask(this));
	if (!ScratchPad) return;
	
	AActor* Actor = Context->GetSelfActor();
	if (IsValid(Actor))
	{
		USPCharacterMovementComponent* MovementComponent = Actor->FindComponentByClass<USPCharacterMovementComponent>();
		ASPGameMonsterBase* Monster = Cast<ASPGameMonsterBase>(Actor);
		if (IsValid(MovementComponent))
		{
			if (!IsValid(Monster))
			{
				return;
			}
			FVector CharacterLoc = Monster->GetActorLocation();
			FHitResult HitResult;
			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.Add(Monster);

			bool bIsHit = false;

			FVector Forward = Monster->GetActorForwardVector();
			float HalfHeight = Monster->GetCapsuleHalfHeight();
			// FVector FrontLoc = CharacterLoc + Forward * FrontBehindLength;
			// FVector BehindLoc = CharacterLoc - Forward * FrontBehindLength;
			bIsHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), FVector(CharacterLoc.X, CharacterLoc.Y, CharacterLoc.Z),
														   CharacterLoc + ScratchPad->VelocityValue * deltaTime,
														   UEngineTypes::ConvertToTraceType(ECC_Visibility), false,
														   ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);

			if (!bIsHit)
			{
				// MovementComponent->Velocity = ScratchPad->VelocityValue;
				MovementComponent->MoveUpdatedComponent(ScratchPad->VelocityValue * deltaTime, MovementComponent->UpdatedComponent->GetComponentQuat(), true);
				// UE_LOG(LogTemp, Log, TEXT("USPSetVelocityTask::OnTaskTickBP %s Velocity: x:%f y:%f z:%f"), *Context->GetOwner()->GetName(), ScratchPad->VelocityValue.X, ScratchPad->VelocityValue.Y, ScratchPad->VelocityValue.Z);
			}
			else
			{
				MovementComponent->MoveUpdatedComponent((HitResult.ImpactPoint - CharacterLoc), MovementComponent->UpdatedComponent->GetComponentQuat(), true);
				// UE_LOG(LogTemp, Log, TEXT("USPSetVelocityTask::OnTaskTickBP %s impactpoint: x:%f y:%f z:%f"), *Context->GetOwner()->GetName(), HitResult.ImpactPoint.X, HitResult.ImpactPoint.Y, HitResult.ImpactPoint.Z);
			}
		}
	}
}

TStatId USPSetVelocityTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPSetVelocityTask, STATGROUP_USPAbility);
}

void USPSetVelocityTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);

	const int32 ShouldSetVelocity = Context->GetIntParameter(TEXT("bShouldSetVelocity"));
	if (ShouldSetVelocity == 1)
	{
		USPSetVelocityTaskScratchPad* ScratchPad = Cast<USPSetVelocityTaskScratchPad>(
			Context->GetScratchPadForTask(this));
		if (!ScratchPad) return;
		ScratchPad->bIsSetVelocity = true;
		ScratchPad->VelocityValue = Context->GetVectorParameter(TEXT("VelocityValue"));
		ScratchPad->StartTime = FApp::GetCurrentTime();
		// UE_LOG(LogTemp, Log, TEXT("USPSetVelocityTask::OnTaskStart %s Velocity: x:%f y:%f z:%f"), *Context->GetOwner()->GetName(), ScratchPad->VelocityValue.X, ScratchPad->VelocityValue.Y, ScratchPad->VelocityValue.Z);

		MoveActorByVelocity(Context, 0.04f);
	}
}

void USPSetVelocityTask::OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context, float deltaTime) const
{
	Super::OnTaskTick(Context, deltaTime);
	
	USPSetVelocityTaskScratchPad* ScratchPad = Cast<USPSetVelocityTaskScratchPad>(
	Context->GetScratchPadForTask(this));
	if (!ScratchPad) return;
	if (!UKismetSystemLibrary::IsDedicatedServer(GetWorld()) && FApp::GetCurrentTime() - ScratchPad->StartTime > ClientSetVelocityDuringTime)
	{
		return;
	}
	if (ScratchPad->bIsSetVelocity)
	{
		MoveActorByVelocity(Context, deltaTime);
	}
}

void USPSetVelocityTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	const EAbleAbilityTaskResult result) const
{
	Super::OnTaskEnd(Context, result);

}

UAbleAbilityTaskScratchPad* USPSetVelocityTask::CreateScratchPad(
	const TWeakObjectPtr<UAbleAbilityContext>& Context) const
{
	if (UAbleAbilityUtilitySubsystem* Subsystem = Context->GetUtilitySubsystem())
	{
		static TSubclassOf<UAbleAbilityTaskScratchPad> ScratchPadClass = USPSetVelocityTaskScratchPad::StaticClass();
		return Subsystem->FindOrConstructTaskScratchPad(ScratchPadClass);
	}

	return NewObject<USPSetVelocityTaskScratchPad>(Context.Get());
}

EAbleAbilityTaskRealm USPSetVelocityTask::GetTaskRealmBP_Implementation() const
{
	return EAbleAbilityTaskRealm::ATR_ClientAndServer;
}

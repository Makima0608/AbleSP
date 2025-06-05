// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SPGame/Skill/Task/SPAdaptiveLandingTask.h"

#include "ableSubSystem.h"
#include "MoeFeatureSPLog.h"
#include "Game/SPGame/Character/SPCharacterMovementComponent.h"
#include "Game/SPGame/Character/SPGameCharacterBase.h"
#include "Game/SPGame/Character/SPGameMonsterBase.h"
#include "Game/SPGame/Utils/SPUtilityLibrary.h"

USPAdaptiveLandingTaskScratchPad::USPAdaptiveLandingTaskScratchPad()
{
}

USPAdaptiveLandingTaskScratchPad::~USPAdaptiveLandingTaskScratchPad()
{
}

void USPAdaptiveLandingTask::MoveActorToAdaptLand(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	float deltaTime) const
{
	float RemainTime = GetEndTime() - Context->GetCurrentTime();
	// MOE_SP_ABILITY_LOG(TEXT("USPAdaptiveLandingTask::MoveActorToAdaptLand RemainTime %f"), RemainTime);
	
	USPAdaptiveLandingTaskScratchPad* ScratchPad = Cast<USPAdaptiveLandingTaskScratchPad>(
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
														   CharacterLoc + FVector(0, 0, -1 * MaxAdaptDistance),
														   UEngineTypes::ConvertToTraceType(ECC_Visibility), false,
														   ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);

			if (!bIsHit)
			{
				float WaterHeight = 0.0f;
				bool IsUpWater = USPUtilityLibrary::GetWaterSurfaceHeightUnderCurrentCharacter(Monster->GetWorld(), Monster, WaterHeight, MaxAdaptDistance, true);
				if (IsUpWater)
				{
					float CurrentVelocityInZ = MovementComponent->Velocity.Z;
					float TargetVelocityInZ = (CharacterLoc.Z - HalfHeight - WaterHeight) / RemainTime;
					// MOE_SP_ABILITY_LOG(TEXT("USPAdaptiveLandingTask::MoveActorToAdaptLand WaterDistance %f"), CharacterLoc.Z - HalfHeight - WaterHeight);

					float AdaptVelocityInZ = TargetVelocityInZ - CurrentVelocityInZ;
					MovementComponent->MoveUpdatedComponent(FVector(0, 0, -1 * AdaptVelocityInZ) * deltaTime, MovementComponent->UpdatedComponent->GetComponentQuat(), true);
				}
				else
				{
					// MovementComponent->Velocity = ScratchPad->VelocityValue;
					float CurrentVelocityInZ = MovementComponent->Velocity.Z;
					float TargetVelocityInZ = (MaxAdaptDistance - HalfHeight) / RemainTime;
					// MOE_SP_ABILITY_LOG(TEXT("USPAdaptiveLandingTask::MoveActorToAdaptLand MaxDistance %f"), MaxAdaptDistance - HalfHeight);

					float AdaptVelocityInZ = TargetVelocityInZ - CurrentVelocityInZ;
					MovementComponent->MoveUpdatedComponent(FVector(0, 0, -1 * AdaptVelocityInZ) * deltaTime, MovementComponent->UpdatedComponent->GetComponentQuat(), true);
					// UE_LOG(LogTemp, Log, TEXT("USPSetVelocityTask::OnTaskTickBP %s Velocity: x:%f y:%f z:%f"), *Context->GetOwner()->GetName(), ScratchPad->VelocityValue.X, ScratchPad->VelocityValue.Y, ScratchPad->VelocityValue.Z);
				}
			}
			else
			{
				float CurrentVelocityInZ = MovementComponent->Velocity.Z;
				float TargetLandDistance = CharacterLoc.Z - HalfHeight - HitResult.ImpactPoint.Z;
				// MOE_SP_ABILITY_LOG(TEXT("USPAdaptiveLandingTask::MoveActorToAdaptLand ActualDistance %f"), CharacterLoc.Z - HalfHeight - HitResult.ImpactPoint.Z);

				float TargetVelocityInZ = TargetLandDistance / RemainTime;
				float AdaptVelocityInZ = TargetVelocityInZ - CurrentVelocityInZ;
				MovementComponent->MoveUpdatedComponent(FVector(0, 0, -1 * AdaptVelocityInZ) * deltaTime, MovementComponent->UpdatedComponent->GetComponentQuat(), true);
				// UE_LOG(LogTemp, Log, TEXT("USPSetVelocityTask::OnTaskTickBP %s impactpoint: x:%f y:%f z:%f"), *Context->GetOwner()->GetName(), HitResult.ImpactPoint.X, HitResult.ImpactPoint.Y, HitResult.ImpactPoint.Z);
			}
		}
	}
}

void USPAdaptiveLandingTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);
	OnTaskStartBP(Context.Get());
}

void USPAdaptiveLandingTask::OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const
{
	MOE_SP_ABILITY_LOG(TEXT("USPAdaptiveLandingTask::OnTaskStartBP"));

	USPAdaptiveLandingTaskScratchPad* ScratchPad = Cast<USPAdaptiveLandingTaskScratchPad>(
	Context->GetScratchPadForTask(this));
	if (!ScratchPad) return;
	ScratchPad->DuringTime = 0.0f;

	MoveActorToAdaptLand(Context, 0.04f);
}

void USPAdaptiveLandingTask::OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context, float deltaTime) const
{
	Super::OnTaskTick(Context, deltaTime);
	OnTaskTickBP(const_cast<UAbleAbilityContext*>(Context.Get()), deltaTime);
}

void USPAdaptiveLandingTask::OnTaskTickBP_Implementation(const UAbleAbilityContext* Context, float deltaTime) const
{
	USPAdaptiveLandingTaskScratchPad* ScratchPad = Cast<USPAdaptiveLandingTaskScratchPad>(
		Context->GetScratchPadForTask(this));
	if (!ScratchPad) return;
	ScratchPad->DuringTime += deltaTime;

	MoveActorToAdaptLand(Context, deltaTime);
}

void USPAdaptiveLandingTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	const EAbleAbilityTaskResult result) const
{
	OnTaskEndBP(const_cast<UAbleAbilityContext*>(Context.Get()), result);
	Super::OnTaskEnd(Context, result);
}

void USPAdaptiveLandingTask::OnTaskEndBP_Implementation(const UAbleAbilityContext* Context,
												   const EAbleAbilityTaskResult result) const
{
	USPAdaptiveLandingTaskScratchPad* ScratchPad = Cast<USPAdaptiveLandingTaskScratchPad>(
	Context->GetScratchPadForTask(this));
	if (!ScratchPad) return;
	ScratchPad->DuringTime = 0.0f;
}

TStatId USPAdaptiveLandingTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPAdaptiveLandingTask, STATGROUP_USPAbility);
}

EAbleAbilityTaskRealm USPAdaptiveLandingTask::GetTaskRealm() const
{
	return EAbleAbilityTaskRealm::ATR_ClientAndServer;
}

UAbleAbilityTaskScratchPad* USPAdaptiveLandingTask::CreateScratchPad(
	const TWeakObjectPtr<UAbleAbilityContext>& Context) const
{
	if (UAbleAbilityUtilitySubsystem* Subsystem = Context->GetUtilitySubsystem())
	{
		static TSubclassOf<UAbleAbilityTaskScratchPad> ScratchPadClass =
			USPAdaptiveLandingTaskScratchPad::StaticClass();
		return Subsystem->FindOrConstructTaskScratchPad(ScratchPadClass);
	}

	return NewObject<USPAdaptiveLandingTaskScratchPad>(Context.Get());
}
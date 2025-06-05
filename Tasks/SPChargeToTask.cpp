// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SPGame/Skill/Task/SPChargeToTask.h"

#include "MoeFeatureSPLog.h"
#include "Game/SPGame/Character/SPGameMonsterBase.h"
#include "Game/SPGame/Utils/SPCharacterMovementLibrary.h"

void USPChargeToTask::OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const
{
	UE_LOG(LogSPAbility, Log, TEXT("OnTaskStartBP_Implementation"));

	TArray<TWeakObjectPtr<AActor>> TargetArray;
	GetActorsForTask(TWeakObjectPtr<const UAbleAbilityContext>(Context), TargetArray);

	for (auto& Target : TargetArray)
	{
		if (Target.IsValid())
		{
			AActor* TargetActor = Target.Get();
			ASPGameMonsterBase* TargetMonster = Cast<ASPGameMonsterBase>(TargetActor);
			if (IsValid(TargetMonster))
			{
				OnCharge(Context, TargetMonster);
			}
			else
			{
				ASPGameCharacterBase* TargetCharacter = Cast<ASPGameCharacterBase>(TargetActor);
				if (IsValid(TargetCharacter))
				{
					OnCharge(Context, TargetCharacter);
				}
			}
		}
	}
	
}

void USPChargeToTask::OnTaskTickBP_Implementation(const UAbleAbilityContext* Context, float DeltaTime) const
{
	// const int32* BranchSegmentMaxDistance = FindValueByName<int32>(TEXT("BranchSegmentMaxDistance"));
	// const FName* BranchSegmentName = FindValueByName<FName>(TEXT("BranchSegmentName"));
}

void USPChargeToTask::OnTaskEndBP_Implementation(const UAbleAbilityContext* Context,
												const EAbleAbilityTaskResult result) const
{
	USPMovementHandle* Handler = Cast<USPMovementHandle>(Context->GetUObjectParameter(TEXT("ChargeTo_Handler")));
	if (IsValid(Handler))
	{
		Handler->Abort();
	}
}

void USPChargeToTask::OnCharge(const UAbleAbilityContext* Context, ACharacter* Character) const
{
	ISPActorInterface* SPActor = Cast<ISPActorInterface>(Character);
	if (!SPActor) return;
	
	const FVector* RotateOffset = FindValueByName<FVector>(TEXT("RotateOffset"));
	if (!RotateOffset) return;
	const bool* IgnoreRotationPitch = FindValueByName<bool>(TEXT("IgnoreRotationPitch"));
	if (!IgnoreRotationPitch) return;
	const bool* IgnoreRotationYaw = FindValueByName<bool>(TEXT("IgnoreRotationYaw"));
	if (!IgnoreRotationYaw) return;
	const bool* IgnoreRotationRoll = FindValueByName<bool>(TEXT("IgnoreRotationRoll"));
	if (!IgnoreRotationRoll) return;
	const float* MaxHSpeed = FindValueByName<float>(TEXT("MaxHSpeed"));
	if (!MaxHSpeed) return;
	const float* MaxVSpeed = FindValueByName<float>(TEXT("MaxVSpeed"));
	if (!MaxVSpeed) return;
	const uint8* ChargeType = FindValueByName<uint8>(TEXT("ChargeType"));
	if (!ChargeType) return;
	const float* Time = FindValueByName<float>(TEXT("Time"));
	if (!Time) return;
	const float* ToleranceDistance = FindValueByName<float>(TEXT("ToleranceDistance"));
	if (!ToleranceDistance) return;
	const float* Timeout = FindValueByName<float>(TEXT("Timeout"));
	if (!Timeout) return;
	const bool* UseVertical = FindValueByName<bool>(TEXT("UseVertical"));
	if (!UseVertical) return;
	const bool* VelocityMode = FindValueByName<bool>(TEXT("VelocityMode"));
	if (!VelocityMode) return;
	UCurveFloat* const* HSpeedCurve = FindValueByName<UCurveFloat*>(TEXT("HSpeedCurve"));
	if (!HSpeedCurve) return;
	UCurveFloat* const* VSpeedCurve = FindValueByName<UCurveFloat*>(TEXT("VSpeedCurve"));
	if (!VSpeedCurve) return;
	
	USPMovementHandle* Handler = nullptr;
	if (*ChargeType == 0)	// Forward
	{
		const FSPMovementSpeedCurveParameter Param = USPCharacterMovementLibrary::GetSpeedCurveParameter(*HSpeedCurve, *MaxHSpeed, *VSpeedCurve, *MaxVSpeed, *Time, nullptr, TEXT(""), TEXT(""));
		const FSPMovementConfig Config = USPCharacterMovementLibrary::GetMovementConfigV2(*UseVertical, RotateOffset->X, RotateOffset->Y, RotateOffset->Z, *IgnoreRotationPitch, *IgnoreRotationYaw, *IgnoreRotationRoll, *ToleranceDistance, *Timeout, *VelocityMode);
		Handler = SPActor->MoveForwardByCurve(Param, Config);
	}
	else if (*ChargeType == 1)	// Target
	{
		AActor* TargetActor = GetSingleActorFromTargetType(Context, ATT_TargetActor);
		if (IsValid(TargetActor))
		{
			const FSPMovementSpeedCurveParameter Param = USPCharacterMovementLibrary::GetSpeedCurveParameter(*HSpeedCurve, *MaxHSpeed, *VSpeedCurve, *MaxVSpeed, *Time, nullptr, TEXT(""), TEXT(""));
			const FSPMovementConfig Config = USPCharacterMovementLibrary::GetMovementConfigV2(*UseVertical, RotateOffset->X, RotateOffset->Y, RotateOffset->Z, *IgnoreRotationPitch, *IgnoreRotationYaw, *IgnoreRotationRoll, *ToleranceDistance, *Timeout, *VelocityMode);
			Handler = SPActor->MoveToTargetLocationByCurve(TargetActor->GetActorLocation(), Param, Config);
		}
	}
	else if (*ChargeType == 2)	// FollowTarget
	{
		AActor* TargetActor = GetSingleActorFromTargetType(Context, ATT_TargetActor);
		if (IsValid(TargetActor))
		{
			const FSPMovementSpeedCurveParameter Param = USPCharacterMovementLibrary::GetSpeedCurveParameter(*HSpeedCurve, *MaxHSpeed, *VSpeedCurve, *MaxVSpeed, *Time, nullptr, TEXT(""), TEXT(""));
			const FSPMovementConfig Config = USPCharacterMovementLibrary::GetMovementConfigV2(*UseVertical, RotateOffset->X, RotateOffset->Y, RotateOffset->Z, *IgnoreRotationPitch, *IgnoreRotationYaw, *IgnoreRotationRoll, *ToleranceDistance, *Timeout, *VelocityMode);
			Handler = SPActor->MoveFollowTargetByCurve(TargetActor, Param, Config);
		}
	}
	else if (*ChargeType == 3)	// Aim
	{
		APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
		FVector AimVector = CameraManager->GetCameraRotation().Vector();
		const FSPMovementSpeedCurveParameter Param = USPCharacterMovementLibrary::GetSpeedCurveParameter(*HSpeedCurve, *MaxHSpeed, *VSpeedCurve, *MaxVSpeed, *Time, nullptr, TEXT(""), TEXT(""));
		const FSPMovementConfig Config = USPCharacterMovementLibrary::GetMovementConfigV2(*UseVertical, RotateOffset->X, RotateOffset->Y, RotateOffset->Z, *IgnoreRotationPitch, *IgnoreRotationYaw, *IgnoreRotationRoll, *ToleranceDistance, *Timeout, *VelocityMode);
		Handler = SPActor->MoveWorldDirByCurve(AimVector, Param, Config);
	}
	else if (*ChargeType == 4)	// AlwaysForward
	{
		const FSPMovementSpeedCurveParameter Param = USPCharacterMovementLibrary::GetSpeedCurveParameter(*HSpeedCurve, *MaxHSpeed, *VSpeedCurve, *MaxVSpeed, *Time, nullptr, TEXT(""), TEXT(""));
		FSPMovementConfig Config = USPCharacterMovementLibrary::GetMovementConfigV2(*UseVertical, RotateOffset->X, RotateOffset->Y, RotateOffset->Z, *IgnoreRotationPitch, *IgnoreRotationYaw, *IgnoreRotationRoll, *ToleranceDistance, *Timeout, *VelocityMode);
		Config.bUpdateDir = true;
		Handler = SPActor->MoveForwardByCurve(Param, Config);
	}
	else if (*ChargeType == 5)	// ForwardAndReBound
	{
		const FSPMovementSpeedCurveParameter Param = USPCharacterMovementLibrary::GetSpeedCurveParameter(*HSpeedCurve, *MaxHSpeed, *VSpeedCurve, *MaxVSpeed, *Time, nullptr, TEXT(""), TEXT(""));
		const FSPMovementConfig Config = USPCharacterMovementLibrary::GetMovementConfigV2(*UseVertical, RotateOffset->X, RotateOffset->Y, RotateOffset->Z, *IgnoreRotationPitch, *IgnoreRotationYaw, *IgnoreRotationRoll, *ToleranceDistance, *Timeout, *VelocityMode);
		Handler = SPActor->MoveForwardAndReboundByCurve(Param, Config);
	}

	UAbleAbilityContext* CastContext = const_cast<UAbleAbilityContext*>(Context);
	CastContext->SetUObjectParameter(TEXT("ChargeTo_Handler"), Handler);
}

template <typename ValueType>
const ValueType* USPChargeToTask::FindValueByName(const FName& PropertyName) const
{
	if (const FProperty* Prop = GetClass()->FindPropertyByName(PropertyName))
	{
		return Prop->ContainerPtrToValuePtr<ValueType>(this);
	}
	return nullptr;
}

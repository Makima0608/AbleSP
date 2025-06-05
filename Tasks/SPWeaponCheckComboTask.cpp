// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SPGame/Skill/Task/SPWeaponCheckComboTask.h"

#include "ableSubSystem.h"
#include "MoeFeatureSPLog.h"
#include "Game/SPGame/Character/SPGameCharacterBase.h"
#include "Game/SPGame/Utils/SPGameLibrary.h"
#include "Game/SPGame/Weapon/SPComboMeleeWeapon.h"

USPWeaponCheckComboTaskScratchPad::USPWeaponCheckComboTaskScratchPad()
{
}

USPWeaponCheckComboTaskScratchPad::~USPWeaponCheckComboTaskScratchPad()
{
}

USPWeaponCheckComboTask::USPWeaponCheckComboTask(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
}

USPWeaponCheckComboTask::~USPWeaponCheckComboTask()
{
}

FString USPWeaponCheckComboTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPWeaponCheckComboTask");
}

void USPWeaponCheckComboTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);
	OnTaskStartBP(Context.Get());
}

void USPWeaponCheckComboTask::OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const
{
	USPWeaponCheckComboTaskScratchPad* ScratchPad = Cast<USPWeaponCheckComboTaskScratchPad>(Context->GetScratchPadForTask(this));
	if(ScratchPad == nullptr)
		return;
	
	ScratchPad->bComboTriggered = false;
	ScratchPad->CurrentHoldTime = 0.0f;
	ScratchPad->bPressedOnStart = false;
		
	if (ASPGameCharacterBase* Player = Cast<ASPGameCharacterBase>(Context->GetOwner()))
	{
		if (USPGameCharInventoryComponent* SPInventoryComp = Cast<USPGameCharInventoryComponent>(Player->SPGetInventoryComponent()))
		{
			if (ASPComboMeleeWeapon* ComboWeapon = Cast<ASPComboMeleeWeapon>(SPInventoryComp->GetCrtEquipWeapon()))
			{
				ScratchPad->SPInventoryComp = SPInventoryComp;
				ScratchPad->ComboWeapon = ComboWeapon;
				ScratchPad->bPressedOnStart = SPInventoryComp->GetIsPressFireButton();
				if (InputMode == ESPComboInputMode::Auto)
				{
					//自动模式，直接激活连击
					ActivateCombo(ComboWeapon);
				}
			}
		}
	}
}

void USPWeaponCheckComboTask::OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
                                         float deltaTime) const
{
	Super::OnTaskTick(Context, deltaTime);
	OnTaskTickBP(Context.Get(), deltaTime);
}

void USPWeaponCheckComboTask::OnTaskTickBP_Implementation(const UAbleAbilityContext* Context, float deltaTime) const
{
	USPWeaponCheckComboTaskScratchPad* ScratchPad = Cast<USPWeaponCheckComboTaskScratchPad>(Context->GetScratchPadForTask(this));
	if(ScratchPad == nullptr)
		return;
	
	if (ScratchPad->ComboWeapon.IsValid() && InputMode != ESPComboInputMode::Auto)
	{
		if (ScratchPad->SPInventoryComp.IsValid())
		{
			if (ScratchPad->SPInventoryComp->GetIsPressFireButton())
			{
				bool bActivated = true;
				if (ScratchPad->bPressedOnStart && InputMode == ESPComboInputMode::Hold)
				{
					ScratchPad->CurrentHoldTime += deltaTime;
					if (ScratchPad->CurrentHoldTime < HoldTime)
					{
						bActivated = false;
					}
				}
				if (ScratchPad->ComboWeapon->CheckIsComboActive())
				{
					// 武器当前还处于当次连击中
					bActivated = false;
				}
				if (bActivated)
				{
					ScratchPad->bComboTriggered = true;
					if (ActivateMode != ESPComboActiveMode::AtEnd)
					{
						// 非结束时激活连击类型，直接激活连击
						ActivateCombo(ScratchPad->ComboWeapon.Get());
					}
				}
			}
			else
			{
				ScratchPad->bPressedOnStart = false;
			}
		}
	}
}

void USPWeaponCheckComboTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	const EAbleAbilityTaskResult result) const
{
	Super::OnTaskEnd(Context, result);
	OnTaskEndBP(Context.Get(), result);
}

void USPWeaponCheckComboTask::OnTaskEndBP_Implementation(const UAbleAbilityContext* Context,
	                 const EAbleAbilityTaskResult result) const
{
	USPWeaponCheckComboTaskScratchPad* ScratchPad = Cast<USPWeaponCheckComboTaskScratchPad>(Context->GetScratchPadForTask(this));
	if(ScratchPad == nullptr)
		return;

	if (ScratchPad->ComboWeapon.IsValid())
	{
		if (result == EAbleAbilityTaskResult::Successful && ActivateMode == ESPComboActiveMode::AtEnd && ScratchPad->bComboTriggered)
		{
			// Task成功结束，连击激活模式是结束时，过程中触发了连击
			ActivateCombo(ScratchPad->ComboWeapon.Get());
		}
	}
	ScratchPad->ComboWeapon.Reset();
	ScratchPad->SPInventoryComp.Reset();
	ScratchPad->bComboTriggered = false;
	ScratchPad->CurrentHoldTime = 0.0f;
	ScratchPad->bPressedOnStart = false;
}

TStatId USPWeaponCheckComboTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPWeaponCheckComboTask, STATGROUP_USPAbility);
}

UAbleAbilityTaskScratchPad* USPWeaponCheckComboTask::CreateScratchPad(
	const TWeakObjectPtr<UAbleAbilityContext>& Context) const
{
	if (UAbleAbilityUtilitySubsystem* Subsystem = Context->GetUtilitySubsystem())
	{
		static TSubclassOf<UAbleAbilityTaskScratchPad> ScratchPadClass = USPWeaponCheckComboTaskScratchPad::StaticClass();
		return Subsystem->FindOrConstructTaskScratchPad(ScratchPadClass);
	}
	return NewObject<USPWeaponCheckComboTaskScratchPad>(Context.Get());
}

void USPWeaponCheckComboTask::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	if(Ar.IsLoading() || Ar.IsSaving())
	{
		// bResetCombo废弃，载入、保存时，更改IndexMode
		if (bResetCombo)
		{
			IndexMode = ESPComboIndexMode::First;
		}
	}
}

bool USPWeaponCheckComboTask::ActivateCombo(ASPComboMeleeWeapon* Weapon) const
{
	if (Weapon)
	{
		const int32 Index = GetComboIndex(Weapon);
		Weapon->ActiveCombo(Index);
		return true;
	}
	return false;
}

int32 USPWeaponCheckComboTask::GetComboIndex(const ASPComboMeleeWeapon* Weapon) const
{
	if (Weapon)
	{
		const int32 CurrentComboIndex = Weapon->GetCurrentComboIndex();
		const int32 MaxComboIndex = Weapon->GetMaxComboCount() - 1;
		
		if (MaxComboIndex < 0)
		{
			MOE_SP_ABILITY_LOG(TEXT("[SPAbility] USPWeaponCheckComboTask::GetComboIndex MaxComboIndex incorrect."));
			return -1;
		}

		int32 NextComboIndex = -1;
		if (IndexMode == ESPComboIndexMode::First)
		{
			NextComboIndex = 0;
		}
		else if (IndexMode == ESPComboIndexMode::Last)
		{
			NextComboIndex = MaxComboIndex;
		}
		else if (IndexMode == ESPComboIndexMode::Next)
		{
			NextComboIndex = CurrentComboIndex + 1;
			if (NextComboIndex > MaxComboIndex)
			{
				// 超过最大连击段数，从头开始
				NextComboIndex = 0;
			}
		}
		else if (IndexMode == ESPComboIndexMode::Prev)
		{
			NextComboIndex = CurrentComboIndex - 1;
			if (NextComboIndex < 0)
			{
				// 小于最小连击段数，从尾开始
				NextComboIndex = MaxComboIndex;
			}
		}
		else if (IndexMode == ESPComboIndexMode::Index)
		{
			NextComboIndex = ComboIndex;
		}

		// Index限制在有效范围内
		const int32 FinalIndex = FMath::Clamp(NextComboIndex, 0, MaxComboIndex);
		return FinalIndex;
	}
	return -1;
}

EAbleAbilityTaskRealm USPWeaponCheckComboTask::GetTaskRealmBP_Implementation() const
{
	return EAbleAbilityTaskRealm::ATR_Client;
}

bool USPWeaponCheckComboTask::IsSingleFrameBP_Implementation() const
{
	if (InputMode == ESPComboInputMode::Auto)
	{
		//自动模式，单帧
		return true;
	}
	return false;
}

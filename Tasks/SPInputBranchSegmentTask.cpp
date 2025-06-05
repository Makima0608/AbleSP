// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SPGame/Skill/Task/SPInputBranchSegmentTask.h"

#include "ableAbilityBlueprintLibrary.h"
#include "ableSubSystem.h"
#include "MoeFeatureSPLog.h"
#include "Game/SPGame/Character/SPGameCharacterBase.h"
#include "Game/SPGame/Monster/SPGameCharInventoryComponent.h"
#include "Game/SPGame/Weapon/SPComboMeleeWeapon.h"

USPInputBranchSegmentTaskScratchPad::USPInputBranchSegmentTaskScratchPad()
{
}

USPInputBranchSegmentTaskScratchPad::~USPInputBranchSegmentTaskScratchPad()
{
}

FString USPInputBranchSegmentTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPInputBranchSegmentTask");
}

USPInputBranchSegmentTask::USPInputBranchSegmentTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), bMustPassAllConditions(0)
{
}

bool USPInputBranchSegmentTask::CanStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context, float CurrentTime,
                                         float DeltaTime) const
{
	if (!bCanSimulateRunOnClient)
	{
		const ASPGameCharacterBase* Character = Cast<ASPGameCharacterBase>(Context->GetSelfActor());
		if (IsValid(Character))
		{
			const UWorld* World = Character->GetWorld();
			if (IsValid(World) && World->IsClient() && !Character->IsLocallyControlled())
			{
				return false;
			}
		}
	}
	
	return Super::CanStart(Context, CurrentTime, DeltaTime);
}

void USPInputBranchSegmentTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);
	OnTaskStartBP(const_cast<UAbleAbilityContext*>(Context.Get()));
}

void USPInputBranchSegmentTask::OnTaskStartBP_Implementation(UAbleAbilityContext* Context) const
{
	if (!Context)
	{
		return;
	}

	USPInputBranchSegmentTaskScratchPad* ScratchPad = Cast<USPInputBranchSegmentTaskScratchPad>(Context->GetScratchPadForTask(this));
	if(ScratchPad == nullptr)
		return;
	
	ScratchPad->SegmentIndex = -1;
	ScratchPad->TaskPhase = ESPSegmentTaskPhase::Checking;
	ScratchPad->bIsInputPassed = false;
	ScratchPad->bPressedOnStart = false;
	ScratchPad->CurrentHoldTime = 0.f;
	ScratchPad->bPressedOnStart = IsPressed(InputTag);

	if (CheckMode == ESPInputBranchSegmentCheckMode::Combo)
	{
		if (ASPGameCharacterBase* Player = Cast<ASPGameCharacterBase>(Context->GetOwner()))
		{
			if (USPGameCharInventoryComponent* SPInventoryComp = Cast<USPGameCharInventoryComponent>(Player->SPGetInventoryComponent()))
			{
				if (ASPComboMeleeWeapon* ComboWeapon = Cast<ASPComboMeleeWeapon>(SPInventoryComp->GetCrtEquipWeapon()))
				{
					ScratchPad->ComboWeapon = ComboWeapon;
				}
			}
		}
	}

	UpdateResultData(Context);
}

void USPInputBranchSegmentTask::OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	float deltaTime) const
{
	Super::OnTaskTick(Context, deltaTime);
	OnTaskTickBP(const_cast<UAbleAbilityContext*>(Context.Get()), deltaTime);
}

void USPInputBranchSegmentTask::OnTaskTickBP_Implementation(UAbleAbilityContext* Context, float deltaTime) const
{
	if (!Context) 
	{
		return;
	}

	USPInputBranchSegmentTaskScratchPad* ScratchPad = Cast<USPInputBranchSegmentTaskScratchPad>(Context->GetScratchPadForTask(this));
	if(ScratchPad == nullptr)
		return;

	if (ScratchPad->TaskPhase == ESPSegmentTaskPhase::Checking)
	{
		int32 SegmentIndex = -1;
		if (!ScratchPad->bIsInputPassed)
		{
			if (CheckInput(Context, deltaTime))
			{
				ScratchPad->bIsInputPassed = true;
			}
		}
		const bool Result = ScratchPad->bIsInputPassed && CheckConditions(Context);
		if (Result)
		{
			// 条件通过，开始执行分支跳转
			ScratchPad->TaskPhase = ESPSegmentTaskPhase::Passed;
			if (CheckMode == ESPInputBranchSegmentCheckMode::Combo)
			{
				if (JumpMode == ESPInputBranchSegmentJumpMode::Immediately)
				{
					OnActivateCombo(Context, ScratchPad->ComboWeapon.Get());
				}
			}
			else
			{
				if (UAbleAbilityComponent* AbilityComponent = Context->GetSelfAbilityComponent())
				{
					SegmentIndex = AbilityComponent->FindSegment(Context, SegmentName);
				}
				if (SegmentIndex >= 0 && SegmentIndex != Context->GetActiveSegmentIndex())
				{
					ScratchPad->SegmentIndex = SegmentIndex;
					if (JumpMode == ESPInputBranchSegmentJumpMode::Immediately)
					{
						OnBranchSegment(Context, SegmentIndex);
					}
				}
			}
		}
	}
	else if (ScratchPad->TaskPhase == ESPSegmentTaskPhase::Passed)
	{
		if (CheckMode == ESPInputBranchSegmentCheckMode::Combo)
		{
			if (JumpMode == ESPInputBranchSegmentJumpMode::Immediately)
			{
				OnActivateCombo(Context, ScratchPad->ComboWeapon.Get());
			}
		}
		else
		{
			const int32 SegmentIndex = ScratchPad->SegmentIndex;
			if (SegmentIndex >= 0 && SegmentIndex != Context->GetActiveSegmentIndex())
			{
				if (JumpMode == ESPInputBranchSegmentJumpMode::Immediately)
				{
					OnBranchSegment(Context, SegmentIndex);
				}
			}
		}
	}

	UpdateResultData(Context);
}

void USPInputBranchSegmentTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
                                          const EAbleAbilityTaskResult result) const
{
	Super::OnTaskEnd(Context, result);

	if (Context.IsValid())
	{
		USPInputBranchSegmentTaskScratchPad* ScratchPad = Cast<USPInputBranchSegmentTaskScratchPad>(Context->GetScratchPadForTask(this));
		if(ScratchPad == nullptr)
			return;
	
		if (result == EAbleAbilityTaskResult::Successful)
		{
			if (ScratchPad->TaskPhase == ESPSegmentTaskPhase::Passed && JumpMode == ESPInputBranchSegmentJumpMode::AtEnd)
			{
				// 正常结束时，勾选结束时跳转
				if (CheckMode == ESPInputBranchSegmentCheckMode::Combo)
				{
					OnActivateCombo(Context.Get(), ScratchPad->ComboWeapon.Get());
				}
				else
				{
					OnBranchSegment(Context.Get(), ScratchPad->SegmentIndex);
				}
			}
			else if (ScratchPad->TaskPhase == ESPSegmentTaskPhase::Checking)
			{
				// 成功结束，依然在检测中，则认定失败
				ScratchPad->TaskPhase = ESPSegmentTaskPhase::Failed;
			}
		}

		UpdateResultData(const_cast<UAbleAbilityContext*>(Context.Get()));
		
		ScratchPad->ComboWeapon = nullptr;
	}
}

TStatId USPInputBranchSegmentTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPInputBranchSegmentTask, STATGROUP_USPAbility);
}

void USPInputBranchSegmentTask::Clear()
{
	Super::Clear();
	for (UAbleChannelingBase* Condition : Conditions)
	{
		if (IsValid(Condition)) Condition->MarkPendingKill();
	}
}

void USPInputBranchSegmentTask::OnBranchSegment(const UAbleAbilityContext* Context, int32 SegmentIndex) const
{
	if (Context)
	{
		if (NeedCheckResultData())
		{
			const int32 Result = GetResultData(Context);
			if (Result != 2)
			{
				// 依赖结果阻止跳转
				USPInputBranchSegmentTaskScratchPad* ScratchPad = Cast<USPInputBranchSegmentTaskScratchPad>(Context->GetScratchPadForTask(this));
				ScratchPad->bPriorityBlocked = true;
				MOE_SP_ABILITY_LOG(TEXT("[SPAbility] USPInputBranchSegmentTask::OnBranchSegment blocked by priority(%s %d)."), *DependencyResultName.ToString(), Result);
				return;
			}
		}
		if (SegmentIndex >= 0 && SegmentIndex != Context->GetActiveSegmentIndex())
		{
			Context->GetSelfAbilityComponent()->BranchSegmentFromClient(Context, SegmentIndex, JumpSegmentSetting);
		}
	}
}

bool USPInputBranchSegmentTask::OnActivateCombo(const UAbleAbilityContext* Context, ASPComboMeleeWeapon* Weapon) const
{
	if (NeedCheckResultData())
	{
		const int32 Result = GetResultData(Context);
		if (Result != 2)
		{
			// 依赖结果阻止跳转
			USPInputBranchSegmentTaskScratchPad* ScratchPad = Cast<USPInputBranchSegmentTaskScratchPad>(Context->GetScratchPadForTask(this));
			ScratchPad->bPriorityBlocked = true;
			MOE_SP_ABILITY_LOG(TEXT("[SPAbility] USPInputBranchSegmentTask::OnActivateCombo blocked by priority(%s %d)."), *DependencyResultName.ToString(), Result);
			return false;
		}
	}
	if (Weapon)
	{
		const int32 Index = GetComboIndex(Weapon);
		Weapon->ActiveCombo(Index);
		return true;
	}
	return false;
}

int32 USPInputBranchSegmentTask::GetComboIndex(const ASPComboMeleeWeapon* Weapon) const
{
	if (Weapon)
	{
		const int32 CurrentComboIndex = Weapon->GetCurrentComboIndex();
		const int32 MaxComboIndex = Weapon->GetMaxComboCount() - 1;
		
		if (MaxComboIndex < 0)
		{
			MOE_SP_ABILITY_LOG(TEXT("[SPAbility] USPInputBranchSegmentTask::GetComboIndex MaxComboIndex incorrect."));
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

UAbleAbilityTaskScratchPad* USPInputBranchSegmentTask::CreateScratchPad(
	const TWeakObjectPtr<UAbleAbilityContext>& Context) const
{
	if (UAbleAbilityUtilitySubsystem* Subsystem = Context->GetUtilitySubsystem())
	{
		static TSubclassOf<UAbleAbilityTaskScratchPad> ScratchPadClass = USPInputBranchSegmentTaskScratchPad::StaticClass();
		return Subsystem->FindOrConstructTaskScratchPad(ScratchPadClass);
	}
	return NewObject<USPInputBranchSegmentTaskScratchPad>(Context.Get());
}

void USPInputBranchSegmentTask::UpdateResultData(UAbleAbilityContext* Context) const
{
	if (ResultName.IsNone())
		return;

	if (!Context)
		return;

	USPInputBranchSegmentTaskScratchPad* ScratchPad = Cast<USPInputBranchSegmentTaskScratchPad>(Context->GetScratchPadForTask(this));
	if(ScratchPad == nullptr)
		return;

	if (ScratchPad->TaskPhase == ESPSegmentTaskPhase::Passed)
	{
		Context->SetIntParameter(ResultName, 1);
	}
	else if (ScratchPad->TaskPhase == ESPSegmentTaskPhase::Failed)
	{
		Context->SetIntParameter(ResultName, 2);
	}
	else
	{
		Context->SetIntParameter(ResultName, 0);
	}
}

int32 USPInputBranchSegmentTask::GetResultData(const UAbleAbilityContext* Context) const
{
	if (DependencyResultName.IsNone())
		return 0;

	if (!Context)
		return 0;

	return Context->GetIntParameter(DependencyResultName);
}

bool USPInputBranchSegmentTask::NeedCheckResultData() const
{
	return !DependencyResultName.IsNone();
}

bool USPInputBranchSegmentTask::CheckInput_Implementation(const UAbleAbilityContext* Context, float DeltaTime) const
{
	USPInputBranchSegmentTaskScratchPad* ScratchPad = Cast<USPInputBranchSegmentTaskScratchPad>(Context->GetScratchPadForTask(this));
	if(ScratchPad == nullptr)
		return false;

	bool bResult = false;
	const bool bIsPressButton = IsPressed(InputTag);
	if (InputMode == ESPSegmentInputMode::Pressed && bIsPressButton)
	{
		ScratchPad->bIsInputPassed = true;
		bResult = true;
	}
	else if (InputMode == ESPSegmentInputMode::Released && !bIsPressButton)
	{
		ScratchPad->bIsInputPassed = true;
		bResult = true;
	}
	else if (InputMode == ESPSegmentInputMode::Hold)
	{
		if (bIsPressButton)
		{
			ScratchPad->bPressedOnStart = true;
			ScratchPad->CurrentHoldTime += DeltaTime;
			if (ScratchPad->CurrentHoldTime >= Duration)
			{
				ScratchPad->bIsInputPassed = true;
				bResult = true;
			}
		}
		else
		{
			if (ScratchPad->bPressedOnStart)
			{
				// 长按模式，松手就失败
				ScratchPad->TaskPhase = ESPSegmentTaskPhase::Failed;
			}
		}
	}
	else if (InputMode == ESPSegmentInputMode::UpToDown)
	{
		if (ScratchPad->bPressedOnStart)
		{
			if (!bIsPressButton)
			{
				// 按下状态，需要松开后，才能通过
				ScratchPad->bPressedOnStart = false;
			}
		}
		else
		{
			if (bIsPressButton)
			{
				// 松开状态，按下通过
				ScratchPad->bIsInputPassed = true;
				bResult = true;
			}
		}
	}
	
	return bResult;
}

bool USPInputBranchSegmentTask::CheckConditions_Implementation(const UAbleAbilityContext* Context) const
{
	bool bRet = false;
	if (Conditions.Num() > 0)
	{
		// 检查条件
		UAbleAbilityContext* ConditionContext = const_cast<UAbleAbilityContext*>(Context);
		if (UAbleAbilityBlueprintLibrary::CheckBranchCond(ConditionContext, Conditions, bMustPassAllConditions))
		{
			bRet = true;
		}
	}
	else
	{
		// 没有条件，直接通过
		bRet = true;
	}
	return bRet;
}

BEGIN_EXPORT_REFLECTED_CLASS(USPInputBranchSegmentTask)
ADD_FUNCTION(IsPressed)
END_EXPORT_CLASS(USPInputBranchSegmentTask)
IMPLEMENT_EXPORTED_CLASS(USPInputBranchSegmentTask)

#undef LOCTEXT_NAMESPACE
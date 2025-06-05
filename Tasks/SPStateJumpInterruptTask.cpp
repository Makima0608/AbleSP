// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SPGame/Skill/Task/SPStateJumpInterruptTask.h"

#include "Game/SPGame/Character/SPGameCharacterBase.h"
#include "Game/SPGame/State/Action/SPCharActionStateBase.h"
#include "Game/SPGame/State/Motion/SPCharMotionStateBase.h"

TStatId USPStateJumpInterruptTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPStateJumpInterruptTask, STATGROUP_USPAbility);
}

FString USPStateJumpInterruptTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPStateJumpInterruptTask");
}

void USPStateJumpInterruptTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);
	OnTaskStartBP(Context.Get());
}

void USPStateJumpInterruptTask::OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const
{

	ASPGameCharacterBase* Player = Cast<ASPGameCharacterBase>(Context->GetOwner());
	if (Player)
	{
		bool bIsLegal = false;
		if (bShouldInActionState)
		{
			int32 CurAction = Player->GetActionState_New();
			EStateIdOffsetType Offset = UMoeStateBase::ReductionOffsetWithID(CurAction);
			uint8 EnumState = UMoeStateBase::ReductionStateIDWithOffset(CurAction, Offset);
			if (CorrectActionState_Main == static_cast<ECharActionState>(EnumState) || CorrectActionState_SP == static_cast<ESPActionState>(EnumState))
			{
				bIsLegal = true;
			}
		}
		if (bShouldInMotionState)
		{
			int32 CurMotion = Player->GetMotionState_New();
			EStateIdOffsetType Offset = UMoeStateBase::ReductionOffsetWithID(CurMotion);
			uint8 EnumState = UMoeStateBase::ReductionStateIDWithOffset(CurMotion, Offset);
			if (CorrectMotionState_Main == static_cast<ECharMotionState>(EnumState) || CorrectMotionState_SP == static_cast<ESPMotionState>(EnumState))
			{
				bIsLegal = true;
			}
		}

		if (bIsLegal)
		{
			if (bShouldInActionState)
			{
				UMoeStateBase* CurrentActionState = Player->GetMoeCharStateComponent()->GetCurrentRuntimeStateByMachineType(static_cast<uint8>(ECharStateMachineType::ActionStateMachine));
				if (USPCharActionStateBase* ActionState = Cast<USPCharActionStateBase>(CurrentActionState))
				{
					// for (auto& Elem : InterruptMotionState)
					// {
					// 	if (ActionState->InterruptMotionStateMap.Contains(Elem))
					// 	{
					// 		ActionState->InterruptMotionStateMap[Elem]++;
					// 	}
					// 	else
					// 	{
					// 		ActionState->InterruptMotionStateMap.Add(Elem, 1);
					// 	}
					// }
					for (auto& Elem : InterruptMotionState_Main)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::Default);
						if (ActionState->InterruptMotionStateMap.Contains(EnumState))
						{
							ActionState->InterruptMotionStateMap[EnumState]++;
						}
						else
						{
							ActionState->InterruptMotionStateMap.Add(EnumState, 1);
						}
					}
					for (auto& Elem : InterruptMotionState_SP)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::SP);
						if (ActionState->InterruptMotionStateMap.Contains(EnumState))
						{
							ActionState->InterruptMotionStateMap[EnumState]++;
						}
						else
						{
							ActionState->InterruptMotionStateMap.Add(EnumState, 1);
						}
					}
				}
			}

			if (bShouldInMotionState)
			{
				UMoeStateBase* CurrentMotionState = Player->GetMoeCharStateComponent()->GetCurrentRuntimeStateByMachineType(static_cast<uint8>(ECharStateMachineType::MotionStateMachine));
				if (USPCharMotionStateBase* MotionState = Cast<USPCharMotionStateBase>(CurrentMotionState))
				{
					// for (auto& Elem : InterruptActionState)
					// {
					// 	if (MotionState->InterruptActionStateMap.Contains(Elem))
					// 	{
					// 		MotionState->InterruptActionStateMap[Elem]++;
					// 	}
					// 	else
					// 	{
					// 		MotionState->InterruptActionStateMap.Add(Elem, 1);
					// 	}
					// }
					for (auto& Elem : InterruptActionState_Main)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::Default);
						if (MotionState->InterruptActionStateMap.Contains(EnumState))
						{
							MotionState->InterruptActionStateMap[EnumState]++;
						}
						else
						{
							MotionState->InterruptActionStateMap.Add(EnumState, 1);
						}
					}
					for (auto& Elem : InterruptActionState_SP)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::SP);
						if (MotionState->InterruptActionStateMap.Contains(EnumState))
						{
							MotionState->InterruptActionStateMap[EnumState]++;
						}
						else
						{
							MotionState->InterruptActionStateMap.Add(EnumState, 1);
						}
					}
				}
			}
		}
	}
}

void USPStateJumpInterruptTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	const EAbleAbilityTaskResult result) const
{
	OnTaskEndBP(Context.Get(), result);
	Super::OnTaskEnd(Context, result);
}

void USPStateJumpInterruptTask::OnTaskEndBP_Implementation(const UAbleAbilityContext* Context,
	                 const EAbleAbilityTaskResult result) const
{
    ASPGameCharacterBase* Player = Cast<ASPGameCharacterBase>(Context->GetOwner());
	if (Player)
	{
		bool bIsLegal = false;
		if (bShouldInActionState && CorrectActionState == Player->GetActionState())
		{
			int32 CurAction = Player->GetActionState_New();
			EStateIdOffsetType Offset = UMoeStateBase::ReductionOffsetWithID(CurAction);
			uint8 EnumState = UMoeStateBase::ReductionStateIDWithOffset(CurAction, Offset);
			if (CorrectActionState_Main == static_cast<ECharActionState>(EnumState) || CorrectActionState_SP == static_cast<ESPActionState>(EnumState))
			{
				bIsLegal = true;
			}
		}
		if (bShouldInMotionState && CorrectMotionState == Player->GetMotionState())
		{
			int32 CurMotion = Player->GetMotionState_New();
			EStateIdOffsetType Offset = UMoeStateBase::ReductionOffsetWithID(CurMotion);
			uint8 EnumState = UMoeStateBase::ReductionStateIDWithOffset(CurMotion, Offset);
			if (CorrectMotionState_Main == static_cast<ECharMotionState>(EnumState) || CorrectMotionState_SP == static_cast<ESPMotionState>(EnumState))
			{
				bIsLegal = true;
			}
		}

		if (bIsLegal)
		{
			if (bShouldInActionState)
			{
				UMoeStateBase* CurrentActionState = Player->GetMoeCharStateComponent()->GetCurrentRuntimeStateByMachineType(static_cast<uint8>(ECharStateMachineType::ActionStateMachine));
				if (USPCharActionStateBase* ActionState = Cast<USPCharActionStateBase>(CurrentActionState))
				{
					// for (auto& Elem : InterruptMotionState)
					// {
					// 	if (ActionState->InterruptMotionStateMap.Contains(Elem))
					// 	{
					// 		ActionState->InterruptMotionStateMap[Elem]--;
					// 		if (ActionState->InterruptMotionStateMap[Elem] == 0)
					// 		{
					// 			ActionState->InterruptMotionStateMap.Remove(Elem);
					// 		}
					// 	}
					// }
					for (auto& Elem : InterruptMotionState_Main)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::Default);
						if (ActionState->InterruptMotionStateMap.Contains(EnumState))
						{
							ActionState->InterruptMotionStateMap[EnumState]--;
							if (ActionState->InterruptMotionStateMap[EnumState] == 0)
							{
								ActionState->InterruptMotionStateMap.Remove(EnumState);
							}
						}
					}
					for (auto& Elem : InterruptMotionState_SP)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::SP);
						if (ActionState->InterruptMotionStateMap.Contains(EnumState))
						{
							ActionState->InterruptMotionStateMap[EnumState]--;
							if (ActionState->InterruptMotionStateMap[EnumState] == 0)
							{
								ActionState->InterruptMotionStateMap.Remove(EnumState);
							}
						}
					}
				}
			}

			if (bShouldInMotionState)
			{
				UMoeStateBase* CurrentMotionState = Player->GetMoeCharStateComponent()->GetCurrentRuntimeStateByMachineType(static_cast<uint8>(ECharStateMachineType::MotionStateMachine));
				if (USPCharMotionStateBase* MotionState = Cast<USPCharMotionStateBase>(CurrentMotionState))
				{
					// for (auto& Elem : InterruptActionState)
					// {
					// 	if (MotionState->InterruptActionStateMap.Contains(Elem))
					// 	{
					// 		MotionState->InterruptActionStateMap[Elem]--;
					// 		if (MotionState->InterruptActionStateMap[Elem] == 0)
					// 		{
					// 			MotionState->InterruptActionStateMap.Remove(Elem);
					// 		}
					// 	}
					// }
					for (auto& Elem : InterruptActionState_Main)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::Default);
						if (MotionState->InterruptActionStateMap.Contains(EnumState))
						{
							MotionState->InterruptActionStateMap[EnumState]--;
							if (MotionState->InterruptActionStateMap[EnumState] == 0)
							{
								MotionState->InterruptActionStateMap.Remove(EnumState);
							}
						}
					}
					for (auto& Elem : InterruptActionState_SP)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::SP);
						if (MotionState->InterruptActionStateMap.Contains(EnumState))
						{
							MotionState->InterruptActionStateMap[EnumState]--;
							if (MotionState->InterruptActionStateMap[EnumState] == 0)
							{
								MotionState->InterruptActionStateMap.Remove(EnumState);
							}
						}
					}
				}
			}
		}
	}
}

EAbleAbilityTaskRealm USPStateJumpInterruptTask::GetTaskRealmBP_Implementation() const { return EAbleAbilityTaskRealm::ATR_ClientAndServer; }

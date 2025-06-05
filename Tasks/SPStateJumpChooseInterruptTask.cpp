// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SPGame/Skill/Task/SPStateJumpChooseInterruptTask.h"

#include "Game/SPGame/Character/SPGameCharacterBase.h"
#include "Game/SPGame/State/Action/SPCharActionStateBase.h"
#include "Game/SPGame/State/Motion/SPCharMotionStateBase.h"

TStatId USPStateJumpChooseInterruptTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPStateJumpChooseInterruptTask, STATGROUP_USPAbility);
}

FString USPStateJumpChooseInterruptTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPStateJumpChooseInterruptTask");
}

void USPStateJumpChooseInterruptTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);
	OnTaskStartBP(Context.Get());
}

void USPStateJumpChooseInterruptTask::OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const
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
					ActionState->bIsDefaultInterrupt++;
					// for (auto& Elem : BlockInterruptMotionState)
					// {
					// 	if (ActionState->BlockInterruptMotionStateMap.Contains(Elem))
					// 	{
					// 		ActionState->BlockInterruptMotionStateMap[Elem]++;
					// 	}
					// 	else
					// 	{
					// 		ActionState->BlockInterruptMotionStateMap.Add(Elem, 1);
					// 	}
					// }
					for (auto& Elem : BlockInterruptMotionState_Main)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::Default);
						if (ActionState->BlockInterruptMotionStateMap.Contains(EnumState))
						{
							ActionState->BlockInterruptMotionStateMap[EnumState]++;
						}
						else
						{
							ActionState->BlockInterruptMotionStateMap.Add(EnumState, 1);
						}
					}
					for (auto& Elem : BlockInterruptMotionState)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::SP);
						if (ActionState->BlockInterruptMotionStateMap.Contains(EnumState))
						{
							ActionState->BlockInterruptMotionStateMap[EnumState]++;
						}
						else
						{
							ActionState->BlockInterruptMotionStateMap.Add(EnumState, 1);
						}
					}
				}
			}

			if (bShouldInMotionState)
			{
				UMoeStateBase* CurrentMotionState = Player->GetMoeCharStateComponent()->GetCurrentRuntimeStateByMachineType(static_cast<uint8>(ECharStateMachineType::MotionStateMachine));
				if (USPCharMotionStateBase* MotionState = Cast<USPCharMotionStateBase>(CurrentMotionState))
				{
					MotionState->bIsDefaultInterrupt++;
					// for (auto& Elem : BlockInterruptActionState)
					// {
					// 	if (MotionState->BlockInterruptActionStateMap.Contains(Elem))
					// 	{
					// 		MotionState->BlockInterruptActionStateMap[Elem]++;
					// 	}
					// 	else
					// 	{
					// 		MotionState->BlockInterruptActionStateMap.Add(Elem, 1);
					// 	}
					// }
					for (auto& Elem : BlockInterruptActionState_Main)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::Default);
						if (MotionState->BlockInterruptActionStateMap.Contains(EnumState))
						{
							MotionState->BlockInterruptActionStateMap[EnumState]++;
						}
						else
						{
							MotionState->BlockInterruptActionStateMap.Add(EnumState, 1);
						}
					}
					for (auto& Elem : BlockInterruptActionState_SP)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::SP);
						if (MotionState->BlockInterruptActionStateMap.Contains(EnumState))
						{
							MotionState->BlockInterruptActionStateMap[EnumState]++;
						}
						else
						{
							MotionState->BlockInterruptActionStateMap.Add(EnumState, 1);
						}
					}
				}
			}
		}
	}
}

void USPStateJumpChooseInterruptTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	const EAbleAbilityTaskResult result) const
{
	OnTaskEndBP(Context.Get(), result);
	Super::OnTaskEnd(Context, result);
}

void USPStateJumpChooseInterruptTask::OnTaskEndBP_Implementation(const UAbleAbilityContext* Context,
	                 const EAbleAbilityTaskResult result) const
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
					ActionState->bIsDefaultInterrupt--;
					// for (auto& Elem : BlockInterruptMotionState)
					// {
					// 	if (ActionState->BlockInterruptMotionStateMap.Contains(Elem))
					// 	{
					// 		ActionState->BlockInterruptMotionStateMap[Elem]--;
					// 		if (ActionState->BlockInterruptMotionStateMap[Elem] == 0)
					// 		{
					// 			ActionState->BlockInterruptMotionStateMap.Remove(Elem);
					// 		}
					// 	}
					// }
					for (auto& Elem : BlockInterruptMotionState_Main)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::Default);
						if (ActionState->BlockInterruptMotionStateMap.Contains(EnumState))
						{
							ActionState->BlockInterruptMotionStateMap[EnumState]--;
							if (ActionState->BlockInterruptMotionStateMap[EnumState] == 0)
							{
								ActionState->BlockInterruptMotionStateMap.Remove(EnumState);
							}
						}
					}
					for (auto& Elem : BlockInterruptMotionState_SP)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::SP);
						if (ActionState->BlockInterruptMotionStateMap.Contains(EnumState))
						{
							ActionState->BlockInterruptMotionStateMap[EnumState]--;
							if (ActionState->BlockInterruptMotionStateMap[EnumState] == 0)
							{
								ActionState->BlockInterruptMotionStateMap.Remove(EnumState);
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
					MotionState->bIsDefaultInterrupt--;
					// for (auto& Elem : BlockInterruptActionState)
					// {
					// 	if (MotionState->BlockInterruptActionStateMap.Contains(Elem))
					// 	{
					// 		MotionState->BlockInterruptActionStateMap[Elem]--;
					// 		if (MotionState->BlockInterruptActionStateMap[Elem] == 0)
					// 		{
					// 			MotionState->BlockInterruptActionStateMap.Remove(Elem);
					// 		}
					// 	}
					// }
					for (auto& Elem : BlockInterruptActionState_Main)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::Default);
						if (MotionState->BlockInterruptActionStateMap.Contains(EnumState))
						{
							MotionState->BlockInterruptActionStateMap[EnumState]--;
							if (MotionState->BlockInterruptActionStateMap[EnumState] == 0)
							{
								MotionState->BlockInterruptActionStateMap.Remove(EnumState);
							}
						}
					}
					for (auto& Elem : BlockInterruptActionState_SP)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::SP);
						if (MotionState->BlockInterruptActionStateMap.Contains(EnumState))
						{
							MotionState->BlockInterruptActionStateMap[EnumState]--;
							if (MotionState->BlockInterruptActionStateMap[EnumState] == 0)
							{
								MotionState->BlockInterruptActionStateMap.Remove(EnumState);
							}
						}
					}
				}
			}
		}
	}
}

EAbleAbilityTaskRealm USPStateJumpChooseInterruptTask::GetTaskRealmBP_Implementation() const { return EAbleAbilityTaskRealm::ATR_ClientAndServer; }

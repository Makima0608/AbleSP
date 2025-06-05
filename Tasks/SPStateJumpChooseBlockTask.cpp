// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SPGame/Skill/Task/SPStateJumpChooseBlockTask.h"

#include "MoeFeatureSPLog.h"
#include "Game/SPGame/Character/SPGameCharacterBase.h"
#include "Game/SPGame/State/Action/SPCharActionStateBase.h"
#include "Game/SPGame/State/Motion/SPCharMotionStateBase.h"

TStatId USPStateJumpChooseBlockTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPStateJumpChooseBlockTask, STATGROUP_USPAbility);
}

FString USPStateJumpChooseBlockTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPStateJumpChooseBlockTask");
}

void USPStateJumpChooseBlockTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);
	OnTaskStartBP(Context.Get());
}

void USPStateJumpChooseBlockTask::OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const
{
	MOE_SP_ABILITY_LOG(TEXT("USPStateJumpChooseBlockTask::OnTaskStartBP"));
	
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
					// for (auto& Elem : BlockActionState)
					// {
					// 	if (ActionState->BlockActionStateMap.Contains(Elem))
					// 	{
					// 		ActionState->BlockActionStateMap[Elem]++;
					// 	}
					// 	else
					// 	{
					// 		ActionState->BlockActionStateMap.Add(Elem, 1);
					// 	}
					// }
					for (auto& Elem : BlockActionState_Main)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::Default);
						if (ActionState->BlockActionStateMap.Contains(EnumState))
						{
							ActionState->BlockActionStateMap[EnumState]++;
						}
						else
						{
							ActionState->BlockActionStateMap.Add(EnumState, 1);
						}
					}
					for (auto& Elem : BlockActionState_SP)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::SP);
						if (ActionState->BlockActionStateMap.Contains(EnumState))
						{
							ActionState->BlockActionStateMap[EnumState]++;
						}
						else
						{
							ActionState->BlockActionStateMap.Add(EnumState, 1);
						}
					}
					// for (auto& Elem : BlockMotionState)
					// {
					// 	if (ActionState->BlockMotionStateMap.Contains(Elem))
					// 	{
					// 		ActionState->BlockMotionStateMap[Elem]++;
					// 	}
					// 	else
					// 	{
					// 		ActionState->BlockMotionStateMap.Add(Elem, 1);
					// 	}
					// }
					for (auto& Elem : BlockMotionState_Main)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::Default);
						if (ActionState->BlockMotionStateMap.Contains(EnumState))
						{
							ActionState->BlockMotionStateMap[EnumState]++;
						}
						else
						{
							ActionState->BlockMotionStateMap.Add(EnumState, 1);
						}
					}
					for (auto& Elem : BlockMotionState_SP)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::SP);
						if (ActionState->BlockMotionStateMap.Contains(EnumState))
						{
							ActionState->BlockMotionStateMap[EnumState]++;
						}
						else
						{
							ActionState->BlockMotionStateMap.Add(EnumState, 1);
						}
					}
				}
			}

			if (bShouldInMotionState)
			{
				UMoeStateBase* CurrentMotionState = Player->GetMoeCharStateComponent()->GetCurrentRuntimeStateByMachineType(static_cast<uint8>(ECharStateMachineType::MotionStateMachine));
				if (USPCharMotionStateBase* MotionState = Cast<USPCharMotionStateBase>(CurrentMotionState))
				{
					// for (auto& Elem : BlockActionState)
					// {
					// 	if (MotionState->BlockActionStateMap.Contains(Elem))
					// 	{
					// 		MotionState->BlockActionStateMap[Elem]++;
					// 	}
					// 	else
					// 	{
					// 		MotionState->BlockActionStateMap.Add(Elem, 1);
					// 	}
					// }
					for (auto& Elem : BlockActionState_Main)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::Default);
						if (MotionState->BlockActionStateMap.Contains(EnumState))
						{
							MotionState->BlockActionStateMap[EnumState]++;
						}
						else
						{
							MotionState->BlockActionStateMap.Add(EnumState, 1);
						}
					}
					for (auto& Elem : BlockActionState_SP)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::SP);
						if (MotionState->BlockActionStateMap.Contains(EnumState))
						{
							MotionState->BlockActionStateMap[EnumState]++;
						}
						else
						{
							MotionState->BlockActionStateMap.Add(EnumState, 1);
						}
					}
					// for (auto& Elem : BlockMotionState)
					// {
					// 	if (MotionState->BlockMotionStateMap.Contains(Elem))
					// 	{
					// 		MotionState->BlockMotionStateMap[Elem]++;
					// 	}
					// 	else
					// 	{
					// 		MotionState->BlockMotionStateMap.Add(Elem, 1);
					// 	}
					// }
					for (auto& Elem : BlockMotionState_Main)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::Default);
						if (MotionState->BlockMotionStateMap.Contains(EnumState))
						{
							MotionState->BlockMotionStateMap[EnumState]++;
						}
						else
						{
							MotionState->BlockMotionStateMap.Add(EnumState, 1);
						}
					}
					for (auto& Elem : BlockMotionState_SP)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::SP);
						if (MotionState->BlockMotionStateMap.Contains(EnumState))
						{
							MotionState->BlockMotionStateMap[EnumState]++;
						}
						else
						{
							MotionState->BlockMotionStateMap.Add(EnumState, 1);
						}
					}
				}
			}
		}
	}
}

void USPStateJumpChooseBlockTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	const EAbleAbilityTaskResult result) const
{
	OnTaskEndBP(Context.Get(), result);
	Super::OnTaskEnd(Context, result);
}

void USPStateJumpChooseBlockTask::OnTaskEndBP_Implementation(const UAbleAbilityContext* Context,
	                 const EAbleAbilityTaskResult result) const
{
	MOE_SP_ABILITY_LOG(TEXT("USPStateJumpChooseBlockTask::OnTaskEndBP"));

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
					// for (auto& Elem : BlockActionState)
					// {
					// 	if (ActionState->BlockActionStateMap.Contains(Elem))
					// 	{
					// 		ActionState->BlockActionStateMap[Elem]--;
					// 		if (ActionState->BlockActionStateMap[Elem] == 0)
					// 		{
					// 			ActionState->BlockActionStateMap.Remove(Elem);
					// 		}
					// 	}
					// }
					for (auto& Elem : BlockActionState_Main)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::Default);
						if (ActionState->BlockActionStateMap.Contains(EnumState))
						{
							ActionState->BlockActionStateMap[EnumState]--;
							if (ActionState->BlockActionStateMap[EnumState] == 0)
							{
								ActionState->BlockActionStateMap.Remove(EnumState);
							}
						}
					}
					for (auto& Elem : BlockActionState_SP)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::SP);
						if (ActionState->BlockActionStateMap.Contains(EnumState))
						{
							ActionState->BlockActionStateMap[EnumState]--;
							if (ActionState->BlockActionStateMap[EnumState] == 0)
							{
								ActionState->BlockActionStateMap.Remove(EnumState);
							}
						}
					}
					// for (auto& Elem : BlockMotionState)
					// {
					// 	if (ActionState->BlockMotionStateMap.Contains(Elem))
					// 	{
					// 		ActionState->BlockMotionStateMap[Elem]--;
					// 		if (ActionState->BlockMotionStateMap[Elem] == 0)
					// 		{
					// 			ActionState->BlockMotionStateMap.Remove(Elem);
					// 		}
					// 	}
					// }
					for (auto& Elem : BlockMotionState_Main)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::Default);
						if (ActionState->BlockMotionStateMap.Contains(EnumState))
						{
							ActionState->BlockMotionStateMap[EnumState]--;
							if (ActionState->BlockMotionStateMap[EnumState] == 0)
							{
								ActionState->BlockMotionStateMap.Remove(EnumState);
							}
						}
					}
					for (auto& Elem : BlockMotionState_SP)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::SP);
						if (ActionState->BlockMotionStateMap.Contains(EnumState))
						{
							ActionState->BlockMotionStateMap[EnumState]--;
							if (ActionState->BlockMotionStateMap[EnumState] == 0)
							{
								ActionState->BlockMotionStateMap.Remove(EnumState);
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
					// for (auto& Elem : BlockActionState)
					// {
					// 	if (MotionState->BlockActionStateMap.Contains(Elem))
					// 	{
					// 		MotionState->BlockActionStateMap[Elem]--;
					// 		if (MotionState->BlockActionStateMap[Elem] == 0)
					// 		{
					// 			MotionState->BlockActionStateMap.Remove(Elem);
					// 		}
					// 	}
					// }
					for (auto& Elem : BlockActionState_Main)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::Default);
						if (MotionState->BlockActionStateMap.Contains(EnumState))
						{
							MotionState->BlockActionStateMap[EnumState]--;
							if (MotionState->BlockActionStateMap[EnumState] == 0)
							{
								MotionState->BlockActionStateMap.Remove(EnumState);
							}
						}
					}
					for (auto& Elem : BlockActionState_SP)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::SP);
						if (MotionState->BlockActionStateMap.Contains(EnumState))
						{
							MotionState->BlockActionStateMap[EnumState]--;
							if (MotionState->BlockActionStateMap[EnumState] == 0)
							{
								MotionState->BlockActionStateMap.Remove(EnumState);
							}
						}
					}
					// for (auto& Elem : BlockMotionState)
					// {
					// 	if (MotionState->BlockMotionStateMap.Contains(Elem))
					// 	{
					// 		MotionState->BlockMotionStateMap[Elem]--;
					// 		if (MotionState->BlockMotionStateMap[Elem] == 0)
					// 		{
					// 			MotionState->BlockMotionStateMap.Remove(Elem);
					// 		}
					// 	}
					// }
					for (auto& Elem : BlockMotionState_Main)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::Default);
						if (MotionState->BlockMotionStateMap.Contains(EnumState))
						{
							MotionState->BlockMotionStateMap[EnumState]--;
							if (MotionState->BlockMotionStateMap[EnumState] == 0)
							{
								MotionState->BlockMotionStateMap.Remove(EnumState);
							}
						}
					}
					for (auto& Elem : BlockMotionState_SP)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::SP);
						if (MotionState->BlockMotionStateMap.Contains(EnumState))
						{
							MotionState->BlockMotionStateMap[EnumState]--;
							if (MotionState->BlockMotionStateMap[EnumState] == 0)
							{
								MotionState->BlockMotionStateMap.Remove(EnumState);
							}
						}
					}
				}
			}
		}
	}
}

EAbleAbilityTaskRealm USPStateJumpChooseBlockTask::GetTaskRealmBP_Implementation() const { return EAbleAbilityTaskRealm::ATR_ClientAndServer; }

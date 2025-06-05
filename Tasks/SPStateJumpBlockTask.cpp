// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SPGame/Skill/Task/SPStateJumpBlockTask.h"

#include "Game/SPGame/Character/SPGameCharacterBase.h"
#include "Game/SPGame/State/Action/SPCharActionStateBase.h"
#include "Game/SPGame/State/Motion/SPCharMotionStateBase.h"

TStatId USPStateJumpBlockTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPStateJumpBlockTask, STATGROUP_USPAbility);
}

FString USPStateJumpBlockTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPStateJumpBlockTask");
}

void USPStateJumpBlockTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);
	OnTaskStartBP(Context.Get());
}

void USPStateJumpBlockTask::OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const
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
					ActionState->bIsDefaultBlock++;
					// for (auto& Elem : OpenActionState)
					// {
					// 	if (ActionState->OpenActionStateMap.Contains(Elem))
					// 	{
					// 		ActionState->OpenActionStateMap[Elem]++;
					// 	}
					// 	else
					// 	{
					// 		ActionState->OpenActionStateMap.Add(Elem, 1);
					// 	}
					// }
					for (auto& Elem : OpenActionState_Main)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::Default);
						if (ActionState->OpenActionStateMap.Contains(EnumState))
						{
							ActionState->OpenActionStateMap[EnumState]++;
						}
						else
						{
							ActionState->OpenActionStateMap.Add(EnumState, 1);
						}
					}
					for (auto& Elem : OpenActionState_SP)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::SP);
						if (ActionState->OpenActionStateMap.Contains(EnumState))
						{
							ActionState->OpenActionStateMap[EnumState]++;
						}
						else
						{
							ActionState->OpenActionStateMap.Add(EnumState, 1);
						}
					}
					// for (auto& Elem : OpenMotionState)
					// {
					// 	if (ActionState->OpenMotionStateMap.Contains(Elem))
					// 	{
					// 		ActionState->OpenMotionStateMap[Elem]++;
					// 	}
					// 	else
					// 	{
					// 		ActionState->OpenMotionStateMap.Add(Elem, 1);
					// 	}
					// }
					for (auto& Elem : OpenMotionState_Main)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::Default);
						if (ActionState->OpenMotionStateMap.Contains(EnumState))
						{
							ActionState->OpenMotionStateMap[EnumState]++;
						}
						else
						{
							ActionState->OpenMotionStateMap.Add(EnumState, 1);
						}
					}
					for (auto& Elem : OpenMotionState_SP)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::SP);
						if (ActionState->OpenMotionStateMap.Contains(EnumState))
						{
							ActionState->OpenMotionStateMap[EnumState]++;
						}
						else
						{
							ActionState->OpenMotionStateMap.Add(EnumState, 1);
						}
					}
				}
			}

			if (bShouldInMotionState)
			{
				UMoeStateBase* CurrentMotionState = Player->GetMoeCharStateComponent()->GetCurrentRuntimeStateByMachineType(static_cast<uint8>(ECharStateMachineType::MotionStateMachine));
				if (USPCharMotionStateBase* MotionState = Cast<USPCharMotionStateBase>(CurrentMotionState))
				{
					MotionState->bIsDefaultBlock++;
					// for (auto& Elem : OpenActionState)
					// {
					// 	if (MotionState->OpenActionStateMap.Contains(Elem))
					// 	{
					// 		MotionState->OpenActionStateMap[Elem]++;
					// 	}
					// 	else
					// 	{
					// 		MotionState->OpenActionStateMap.Add(Elem, 1);
					// 	}
					// }
					for (auto& Elem : OpenActionState_Main)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::Default);
						if (MotionState->OpenActionStateMap.Contains(EnumState))
						{
							MotionState->OpenActionStateMap[EnumState]++;
						}
						else
						{
							MotionState->OpenActionStateMap.Add(EnumState, 1);
						}
					}
					for (auto& Elem : OpenActionState_SP)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::SP);
						if (MotionState->OpenActionStateMap.Contains(EnumState))
						{
							MotionState->OpenActionStateMap[EnumState]++;
						}
						else
						{
							MotionState->OpenActionStateMap.Add(EnumState, 1);
						}
					}
					// for (auto& Elem : OpenMotionState)
					// {
					// 	if (MotionState->OpenMotionStateMap.Contains(Elem))
					// 	{
					// 		MotionState->OpenMotionStateMap[Elem]++;
					// 	}
					// 	else
					// 	{
					// 		MotionState->OpenMotionStateMap.Add(Elem, 1);
					// 	}
					// }
					for (auto& Elem : OpenMotionState_Main)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::Default);
						if (MotionState->OpenMotionStateMap.Contains(EnumState))
						{
							MotionState->OpenMotionStateMap[EnumState]++;
						}
						else
						{
							MotionState->OpenMotionStateMap.Add(EnumState, 1);
						}
					}
					for (auto& Elem : OpenMotionState_SP)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::SP);
						if (MotionState->OpenMotionStateMap.Contains(EnumState))
						{
							MotionState->OpenMotionStateMap[EnumState]++;
						}
						else
						{
							MotionState->OpenMotionStateMap.Add(EnumState, 1);
						}
					}
				}
			}
		}
	}
}

void USPStateJumpBlockTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	const EAbleAbilityTaskResult result) const
{
	OnTaskEndBP(Context.Get(), result);
	Super::OnTaskEnd(Context, result);
}

void USPStateJumpBlockTask::OnTaskEndBP_Implementation(const UAbleAbilityContext* Context,
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
					ActionState->bIsDefaultBlock--;
					// for (auto& Elem : OpenActionState)
					// {
					// 	if (ActionState->OpenActionStateMap.Contains(Elem))
					// 	{
					// 		ActionState->OpenActionStateMap[Elem]--;
					// 		if (ActionState->OpenActionStateMap[Elem] == 0)
					// 		{
					// 			ActionState->OpenActionStateMap.Remove(Elem);
					// 		}
					// 	}
					// }
					for (auto& Elem : OpenActionState_Main)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(
							static_cast<uint8>(Elem), EStateIdOffsetType::Default);
						if (ActionState->OpenActionStateMap.Contains(EnumState))
						{
							ActionState->OpenActionStateMap[EnumState]--;
							if (ActionState->OpenActionStateMap[EnumState] == 0)
							{
								ActionState->OpenActionStateMap.Remove(EnumState);
							}
						}
					}
					for (auto& Elem : OpenActionState_SP)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(
							static_cast<uint8>(Elem), EStateIdOffsetType::SP);
						if (ActionState->OpenActionStateMap.Contains(EnumState))
						{
							ActionState->OpenActionStateMap[EnumState]--;
							if (ActionState->OpenActionStateMap[EnumState] == 0)
							{
								ActionState->OpenActionStateMap.Remove(EnumState);
							}
						}
					}
					// for (auto& Elem : OpenMotionState)
					// {
					// 	if (ActionState->OpenMotionStateMap.Contains(Elem))
					// 	{
					// 		ActionState->OpenMotionStateMap[Elem]--;
					// 		if (ActionState->OpenMotionStateMap[Elem] == 0)
					// 		{
					// 			ActionState->OpenMotionStateMap.Remove(Elem);
					// 		}
					// 	}
					// }
					for (auto& Elem : OpenMotionState_Main)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::Default);
						if (ActionState->OpenMotionStateMap.Contains(EnumState))
						{
							ActionState->OpenMotionStateMap[EnumState]--;
							if (ActionState->OpenMotionStateMap[EnumState] == 0)
							{
								ActionState->OpenMotionStateMap.Remove(EnumState);
							}
						}
					}
					for (auto& Elem : OpenMotionState_SP)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::SP);
						if (ActionState->OpenMotionStateMap.Contains(EnumState))
						{
							ActionState->OpenMotionStateMap[EnumState]--;
							if (ActionState->OpenMotionStateMap[EnumState] == 0)
							{
								ActionState->OpenMotionStateMap.Remove(EnumState);
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
					MotionState->bIsDefaultBlock--;
					// for (auto& Elem : OpenActionState)
					// {
					// 	if (MotionState->OpenActionStateMap.Contains(Elem))
					// 	{
					// 		MotionState->OpenActionStateMap[Elem]--;
					// 		if (MotionState->OpenActionStateMap[Elem] == 0)
					// 		{
					// 			MotionState->OpenActionStateMap.Remove(Elem);
					// 		}
					// 	}
					// }
					for (auto& Elem : OpenActionState_Main)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::Default);
						if (MotionState->OpenActionStateMap.Contains(EnumState))
						{
							MotionState->OpenActionStateMap[EnumState]--;
							if (MotionState->OpenActionStateMap[EnumState] == 0)
							{
								MotionState->OpenActionStateMap.Remove(EnumState);
							}
						}
					}
					for (auto& Elem : OpenActionState_SP)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::SP);
						if (MotionState->OpenActionStateMap.Contains(EnumState))
						{
							MotionState->OpenActionStateMap[EnumState]--;
							if (MotionState->OpenActionStateMap[EnumState] == 0)
							{
								MotionState->OpenActionStateMap.Remove(EnumState);
							}
						}
					}
					// for (auto& Elem : OpenMotionState)
					// {
					// 	if (MotionState->OpenMotionStateMap.Contains(Elem))
					// 	{
					// 		MotionState->OpenMotionStateMap[Elem]--;
					// 		if (MotionState->OpenMotionStateMap[Elem] == 0)
					// 		{
					// 			MotionState->OpenMotionStateMap.Remove(Elem);
					// 		}
					// 	}
					// }
					for (auto& Elem : OpenMotionState_Main)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::Default);
						if (MotionState->OpenMotionStateMap.Contains(EnumState))
						{
							MotionState->OpenMotionStateMap[EnumState]--;
							if (MotionState->OpenMotionStateMap[EnumState] == 0)
							{
								MotionState->OpenMotionStateMap.Remove(EnumState);
							}
						}
					}
					for (auto& Elem : OpenMotionState_SP)
					{
						int32 EnumState = UMoeStateBase::GetStateIDWithOffset(static_cast<uint8>(Elem), EStateIdOffsetType::SP);
						if (MotionState->OpenMotionStateMap.Contains(EnumState))
						{
							MotionState->OpenMotionStateMap[EnumState]--;
							if (MotionState->OpenMotionStateMap[EnumState] == 0)
							{
								MotionState->OpenMotionStateMap.Remove(EnumState);
							}
						}
					}
				}
			}
		}
	}
}

EAbleAbilityTaskRealm USPStateJumpBlockTask::GetTaskRealmBP_Implementation() const { return EAbleAbilityTaskRealm::ATR_ClientAndServer; }

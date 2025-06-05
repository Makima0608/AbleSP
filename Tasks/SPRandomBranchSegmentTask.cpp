// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SPGame/Skill/Task/SPRandomBranchSegmentTask.h"
#include "ableAbility.h"
#include "ableSubSystem.h"
#include "ableAbilityBlueprintLibrary.h"
#include "ableAbilityComponent.h"
#include "Game/SPGame/Skill/Core/SPAbilityFunctionLibrary.h"
#include "Game/SPGame/Utils/SPGameLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(LogSPRandomBranchSegmentTask, Warning, All);

USPRandomBranchSegmentTaskScratchPad::USPRandomBranchSegmentTaskScratchPad()
{
}

USPRandomBranchSegmentTaskScratchPad::~USPRandomBranchSegmentTaskScratchPad()
{
}

USPRandomBranchSegmentTask::USPRandomBranchSegmentTask(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer),
	bMustPassAllConditions(true),
	m_TaskRealm(EAbleAbilityTaskRealm::ATR_Server)
{
}

bool USPRandomBranchSegmentTask::CanStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context, float CurrentTime, float DeltaTime) const
{
	return Super::CanStart(Context, CurrentTime, DeltaTime);
}

FString USPRandomBranchSegmentTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPRandomBranchSegmentTask");
}

void USPRandomBranchSegmentTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);
	OnTaskStartBP(Context.Get());
}

void USPRandomBranchSegmentTask::OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const
{
	if (Context)
	{
		const FAbilitySegmentDefineData* ActiveSegment = Context->GetAbility()->FindSegmentDefineDataByIndex(m_Segment);
		check(ActiveSegment != nullptr);
		const FName ChangeSegmentName = ActiveSegment->m_SegmentName;

		USPRandomBranchSegmentTaskScratchPad* ScratchPad = CastChecked<USPRandomBranchSegmentTaskScratchPad>(Context->GetScratchPadForTask(this));
		if (!ScratchPad) return;

		const int64 ChangeStartTime = UMoeBlueprintLibrary::MoeGetNowUtcMillisecons();
		UAbleAbilityContext* MutableContext = const_cast<UAbleAbilityContext*>(Context);
		FString ChangeStartTimeStr = FString::Printf(TEXT("%lld"), ChangeStartTime);
		MutableContext->SetStringParameter(ChangeSegmentName, ChangeStartTimeStr);
		UE_LOG(LogSPRandomBranchSegmentTask, Warning, TEXT("SPRandomBranchSegmentTask OnTaskStartBP CurSegmentName [%s] : StartTime[%lld] : CurSegmentIndex[%d]"), *ChangeSegmentName.ToString(), ChangeStartTime, ScratchPad->SegmentIndex);
	}
}

void USPRandomBranchSegmentTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context, const EAbleAbilityTaskResult result) const
{
	Super::OnTaskEnd(Context, result);
	OnTaskEndBP(Context.Get(), result);
}

void USPRandomBranchSegmentTask::OnTaskEndBP_Implementation(const UAbleAbilityContext* Context, const EAbleAbilityTaskResult result) const
{
	if (Context)
	{
		USPRandomBranchSegmentTaskScratchPad* ScratchPad = CastChecked<USPRandomBranchSegmentTaskScratchPad>(Context->GetScratchPadForTask(this));
		if (!ScratchPad) return;

		ScratchPad->SegmentIndex = GenerateRandomIndexByWeight(Context, RandomBranchSegmentList);
		UE_LOG(LogSPRandomBranchSegmentTask, Warning, TEXT("SPRandomBranchSegmentTask OnTaskEndBP NextSegmentIndex[%d]"), ScratchPad->SegmentIndex);
		if (RandomBranchSegmentList.IsValidIndex(ScratchPad->SegmentIndex))
		{
			const auto SegmentInfo = RandomBranchSegmentList[ScratchPad->SegmentIndex];
			const FName ChangeSegmentName = SegmentInfo.SegmentName;
			const int32 SegmentIndex = Context->GetSelfAbilityComponent()->FindSegment(Context, ChangeSegmentName);
			//UE_LOG(LogSPRandomBranchSegmentTask, Warning, TEXT("SPRandomBranchSegmentTask BranchSegment in Ability [%s] : SegmentIndex[%d]"), *Context->GetAbility()->GetDisplayName(), SegmentIndex);
			if (SegmentIndex >= 0 && SegmentIndex != Context->GetActiveSegmentIndex())
			{
				if (m_TaskRealm == EAbleAbilityTaskRealm::ATR_Server)
				{
					Context->GetSelfAbilityComponent()->BranchSegment(Context, SegmentIndex, JumpSegmentSetting);
				}
			}
		}
	}
}

TStatId USPRandomBranchSegmentTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPRandomBranchSegmentTask, STATGROUP_USPAbility);
}

EAbleAbilityTaskRealm USPRandomBranchSegmentTask::GetTaskRealmBP_Implementation() const { return m_TaskRealm; } // Client for Auth client, Server for AIs/Proxies.

bool USPRandomBranchSegmentTask::IsFillTimeBP_Implementation() const { return true; }

UAbleAbilityTaskScratchPad* USPRandomBranchSegmentTask::CreateScratchPad(const TWeakObjectPtr<UAbleAbilityContext>& Context) const
{
	if (UAbleAbilityUtilitySubsystem* Subsystem = Context->GetUtilitySubsystem())
	{
		static TSubclassOf<UAbleAbilityTaskScratchPad> ScratchPadClass = USPRandomBranchSegmentTaskScratchPad::StaticClass();
		return Subsystem->FindOrConstructTaskScratchPad(ScratchPadClass);
	}

	return NewObject<USPRandomBranchSegmentTaskScratchPad>(Context.Get());
}

int32 USPRandomBranchSegmentTask::GenerateRandomIndexByWeight(const UAbleAbilityContext* Context, const TArray<FRandomBranchSegmentInfo>& SegmentList) const
{
	// 计算权重总和
	int32 TotalWeight = 0;
	int64 LastChangeTime = 0;
	FRandomBranchSegmentInfo SegmentInfoItem;
	int64 TimeNow = UMoeBlueprintLibrary::MoeGetNowUtcMillisecons();
	UAbleAbilityContext* MutableContext = const_cast<UAbleAbilityContext*>(Context);
	for (int32 Index = 0; Index < SegmentList.Num(); ++Index)
	{
		SegmentInfoItem = SegmentList[Index];
		FString CoolDownMSString =  MutableContext->GetStringParameter(SegmentInfoItem.SegmentName);
		LastChangeTime = CoolDownMSString.IsEmpty() ? 0 : FCString::Atoi64(*CoolDownMSString);
		int64 interval = TimeNow - LastChangeTime;
		//UE_LOG(LogSPRandomBranchSegmentTask, Warning, TEXT("SPRandomBranchSegmentTask::CanbChangeSegment LastChangeTime [%lld] : TimeNow [%lld] : CoolDownMS [%lld] : [%d]"), LastChangeTime, TimeNow, interval, SegmentInfoItem.CoolDownMS)
		if (interval > SegmentInfoItem.CoolDownMS)
		{
			TotalWeight += SegmentInfoItem.Weight;
			//UE_LOG(LogSPRandomBranchSegmentTask, Warning, TEXT("SPRandomBranchSegmentTask GenerateRandomIndexByWeight SegmentName [%s] : Weight [%d]"), *SegmentInfoItem.SegmentName.ToString(), SegmentInfoItem.Weight);
		}
	}
 
	// 生成一个[0, TotalWeight)范围内的随机数
	const int32 RandomNumber = FMath::RandRange(0, TotalWeight);
	UE_LOG(LogSPRandomBranchSegmentTask, Warning, TEXT("SPRandomBranchSegmentTask GenerateRandomIndexByWeight RandomNumber [%d] : TotalWeight [%d]"), RandomNumber, TotalWeight);
	// 累计权重，以确定随机数落在哪个范围内
	int32 CumulativeWeight = 0;
	int32 SelectedIndex = 0;
	for (int32 Index = 0; Index < SegmentList.Num(); ++Index)
	{
		SegmentInfoItem = SegmentList[Index];
		FString CoolDownMSString =  MutableContext->GetStringParameter(SegmentInfoItem.SegmentName);
		LastChangeTime = CoolDownMSString.IsEmpty() ? 0 : FCString::Atoi64(*CoolDownMSString);
		if (TimeNow - LastChangeTime > SegmentInfoItem.CoolDownMS)
		{
			CumulativeWeight += SegmentInfoItem.Weight;
			if (RandomNumber < CumulativeWeight)
			{
				SelectedIndex = Index;
				break;
			}
		}
	}

	UE_LOG(LogSPRandomBranchSegmentTask, Warning, TEXT("SPRandomBranchSegmentTask GenerateRandomIndexByWeight SelectedIndex [%d] , CumulativeWeight [%d]"), SelectedIndex, CumulativeWeight);
	return SelectedIndex;
}

bool USPRandomBranchSegmentTask::CanbChangeSegment(const USPRandomBranchSegmentTaskScratchPad* ScratchPad, const FRandomBranchSegmentInfo SegmentInfo) const
{
	const int64 TimeNow = UMoeBlueprintLibrary::MoeGetNowUtcMillisecons();
	for (auto SpawnInfo : ScratchPad->SpawnInfos)
	{
		if (SegmentInfo.SegmentName == SpawnInfo.SegmentName)
		{
			int64 LastChangeTime = SpawnInfo.LastChangeTime;
			//UE_LOG(LogSPRandomBranchSegmentTask, Warning, TEXT("SPRandomBranchSegmentTask::CanbChangeSegment LastChangeTime [%d] : TimeNow [%d] : CoolDownMS [%d]"), LastChangeTime, TimeNow, SegmentInfo.CoolDownMS);
			if (LastChangeTime >= TimeNow - SegmentInfo.CoolDownMS)
			{
				return false;
			}
		}
	}
	
	return true;
}
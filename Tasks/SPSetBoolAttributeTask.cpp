// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SPGame/Skill/Task/SPSetBoolAttributeTask.h"
#include "Game/SPGame/Gameplay/SPActorInterface.h"
#include "MoeGameLog.h"
#include "Engine/World.h"
#include "Game/SPGame/Character/SPGameCharacterBase.h"
#include "Game/SPGame/Utils/SPGameLibrary.h"
#include "Game/SPGame/Skill/Task/SPSetBoolAttributeTaskScratchPad.h"
#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"
#include "ableSubSystem.h"
#include "Game/SPGame/Utils/SPGameLibrary.h"

//DECLARE_STATS_GROUP(TEXT("SPSkillAbility"), STATGROUP_USPAbility, STATCAT_Advanced);

USPSetBoolAttributeTask::USPSetBoolAttributeTask(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer),
	m_IsDuration(false),
	m_SetValue(false),
	m_RestoreAtEnd(false),
	m_ResetValue(false),
	m_FireEvent(false),
	m_Name(NAME_None),
	m_TaskRealm(EAbleAbilityTaskRealm::ATR_Server)
{
	
}

USPSetBoolAttributeTask::~USPSetBoolAttributeTask()
{
}

FString USPSetBoolAttributeTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPSetBoolAttributeTask");
}

void USPSetBoolAttributeTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);
	OnTaskStartBP(Context.Get());
}

void USPSetBoolAttributeTask::OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const
{
	USPSetBoolAttributeTaskScratchPad* ScratchPad = nullptr;

	AActor* TargetActor = GetSingleActorFromTargetType(Context, m_Target);
	if (m_RestoreAtEnd)
	{
		ScratchPad = Cast<USPSetBoolAttributeTaskScratchPad>(Context->GetScratchPadForTask(this));
		if (!ScratchPad) return;
		//ScratchPad->m_OriginalValue = USPGameLibrary::
	}
	if (TargetActor)
	{
		USPGameLibrary::SetBoolAttribute(TargetActor,m_Attribute.ToString(),m_SetValue);
	}
}

void USPSetBoolAttributeTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	const EAbleAbilityTaskResult result) const
{
	Super::OnTaskEnd(Context, result);
	OnTaskEndBP(Context.Get(), result);
}

void USPSetBoolAttributeTask::OnTaskEndBP_Implementation(const UAbleAbilityContext* Context,
	                 const EAbleAbilityTaskResult result) const
{
	
	if (m_IsDuration)
	{
		AActor* TargetActor = GetSingleActorFromTargetType(Context, m_Target);
		if (TargetActor)
		{
			if(m_RestoreAtEnd)
			{
				USPSetBoolAttributeTaskScratchPad* ScratchPad = Cast<USPSetBoolAttributeTaskScratchPad>(Context->GetScratchPadForTask(this));
				if (ScratchPad)
				{
					USPGameLibrary::SetBoolAttribute(TargetActor,m_Attribute.ToString(),ScratchPad->m_OriginalValue);
				}
			}else
			{
				USPGameLibrary::SetBoolAttribute(TargetActor,m_Attribute.ToString(),m_ResetValue);
			}
		}
	}
}

UAbleAbilityTaskScratchPad* USPSetBoolAttributeTask::CreateScratchPad(const TWeakObjectPtr<UAbleAbilityContext>& Context) const
{
	if (m_IsDuration)
	{
		if (UAbleAbilityUtilitySubsystem* Subsystem = Context->GetUtilitySubsystem())
		{
			static TSubclassOf<UAbleAbilityTaskScratchPad> ScratchPadClass = USPSetBoolAttributeTaskScratchPad::StaticClass();
			return Subsystem->FindOrConstructTaskScratchPad(ScratchPadClass);
		}
		return NewObject<USPSetBoolAttributeTaskScratchPad>(Context.Get());
	}
	return nullptr;
}

TStatId USPSetBoolAttributeTask::GetStatId() const
{
	
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPSetBoolAttributeTask, STATGROUP_USPAbility);
}



void USPSetBoolAttributeTask::BindDynamicDelegates(UAbleAbility* Ability)
{

	Super::BindDynamicDelegates(Ability);

	ABL_BIND_DYNAMIC_PROPERTY(Ability, m_IsDuration, TEXT("IsDuration"));
	ABL_BIND_DYNAMIC_PROPERTY(Ability, m_SetValue, TEXT("SetValue"));
	ABL_BIND_DYNAMIC_PROPERTY(Ability, m_RestoreAtEnd, TEXT("RestoreAtEnd"));
	ABL_BIND_DYNAMIC_PROPERTY(Ability, m_ResetValue, TEXT("ResetValue"));
	ABL_BIND_DYNAMIC_PROPERTY(Ability, m_IsDuration, TEXT("IsDuration"));
	ABL_BIND_DYNAMIC_PROPERTY(Ability, m_Attribute, TEXT("Attribute"));
	
}

#if WITH_EDITOR
FText USPSetBoolAttributeTask::GetDescriptiveTaskName() const
{
	FText MyText = FText::FromString(TEXT("SetAttributeTask"));
	return MyText;
}

FText USPSetBoolAttributeTask::GetRichTextTaskSummary() const
{
	FTextBuilder StringBuilder;
	StringBuilder.AppendLine(Super::GetRichTextTaskSummary());
	return StringBuilder.ToText();
}

EDataValidationResult USPSetBoolAttributeTask::IsTaskDataValid(const UAbleAbility* AbilityContext, const FText& AssetName,TArray<FText>& ValidationErrors)
{
	EDataValidationResult result = EDataValidationResult::Valid;
	return  result;
}
#endif

bool USPSetBoolAttributeTask::IsSingleFrameBP_Implementation() const { return !m_IsDuration; }

EAbleAbilityTaskRealm USPSetBoolAttributeTask::GetTaskRealmBP_Implementation() const { return m_TaskRealm; }

#undef LOCTEXT_NAMESPACE

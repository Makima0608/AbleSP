// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SPGame/Skill/Task/SPShowTargetingUITask.h"

#include "ableSubSystem.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Game/SPGame/Utils/SPGameLibrary.h"
#include "MoeGameplay/Character/BaseMoeGameCharacter.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

USPShowTargetingUITaskScratchPad::USPShowTargetingUITaskScratchPad()
{
}

FString USPShowTargetingUITaskScratchPad::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPShowTargetingUITaskScratchPad");
}

const AActor* USPShowTargetingUITaskScratchPad::GetTraceActorByIndex(const int32 Index)
{
	if (!TraceTargetActors.Contains(Index))
	{
		return nullptr;
	}
	return TraceTargetActors[Index].Get();
}

void USPShowTargetingUITaskScratchPad::AddTraceActorByIndex(int32 Index, AActor* TraceActor)
{
	TraceTargetActors.Add(Index, TraceActor);
}

void USPShowTargetingUITaskScratchPad::RemoveTraceActorByIndex(int32 Index)
{
	if (TraceTargetActors.Contains(Index))
	{
		TraceTargetActors.Remove(Index);
	}
}

void USPShowTargetingUITaskScratchPad::Reset()
{
	TraceTargetActors.Empty();
	bActorTraced = false;
}

bool USPShowTargetingUITaskScratchPad::GetActorTraced() const
{
	return bActorTraced;
}

void USPShowTargetingUITaskScratchPad::SetActorTraced(bool bTraced)
{
	bActorTraced = bTraced;
}

void USPShowTargetingUITask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);
	const ABaseMoeGameCharacter* CharacterOwner = Cast<ABaseMoeGameCharacter>(Context->GetOwner());
	if (!CharacterOwner || !CharacterOwner->IsLocalCharacter())
	{
		return;
	}

	APlayerController* PController = CharacterOwner->GetMoePlayerController();
	if (!PController)
	{
		return;
	}
	
	if (!UIName.IsEmpty())
	{
		const FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(PController);
		float FinalWidth = 0.f;
		float FinalHeight = 0.f;
		if (ScreenType == ESPFilterScreenType::Circle)
		{
			if (ValueType == ESPFilterScreenValueType::Percentage)
			{
				FinalWidth = ViewportSize.Y * Radius * 2.f;
				FinalHeight = FinalWidth;
			}
			else
			{
				FinalWidth = Radius;
				FinalHeight = Radius;
			}
		}
		else
		{
			if (ValueType == ESPFilterScreenValueType::Percentage)
			{
				FinalWidth = ViewportSize.X * Width;
				FinalHeight = ViewportSize.Y * Height;
			}
			else
			{
				FinalWidth = Width;
                FinalHeight = Height;
			}
		}
		ShowTargetingUI(FinalWidth, FinalHeight);
	}
	
	StartTraceTargetActors(Context.Get());
}

void USPShowTargetingUITask::OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context, float deltaTime) const
{
	Super::OnTaskTick(Context, deltaTime);

	if (TargetCount <= 0 || !bTickTrace) return;
	
	const ABaseMoeGameCharacter* CharacterOwner = Cast<ABaseMoeGameCharacter>(Context->GetOwner());
	if (!CharacterOwner || !CharacterOwner->IsLocalCharacter())
	{
		return;
	}

	StartTraceTargetActors(Context.Get());
}

void USPShowTargetingUITask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	const EAbleAbilityTaskResult result) const
{
	Super::OnTaskEnd(Context, result);
	const ABaseMoeGameCharacter* CharacterOwner = Cast<ABaseMoeGameCharacter>(Context->GetOwner());
	if (!CharacterOwner || !CharacterOwner->IsLocalCharacter())
	{
		return;
	}

	if (!UIName.IsEmpty())
	{
		HideTargetingUI();
	}
	
	EndTraceTargetActors(Context.Get());
}

UAbleAbilityTaskScratchPad* USPShowTargetingUITask::CreateScratchPad(
	const TWeakObjectPtr<UAbleAbilityContext>& Context) const
{
	if (UAbleAbilityUtilitySubsystem* Subsystem = Context->GetUtilitySubsystem())
	{
		static TSubclassOf<UAbleAbilityTaskScratchPad> ScratchPadClass = USPShowTargetingUITaskScratchPad::StaticClass();
		return Subsystem->FindOrConstructTaskScratchPad(ScratchPadClass);
	}

	return NewObject<USPShowTargetingUITaskScratchPad>(Context.Get());
}

void USPShowTargetingUITask::ResetScratchPad(UAbleAbilityTaskScratchPad* ScratchPad) const
{
	Super::ResetScratchPad(ScratchPad);
	if (USPShowTargetingUITaskScratchPad* CustomScratchPad = Cast<USPShowTargetingUITaskScratchPad>(ScratchPad))
	{
		CustomScratchPad->Reset();
	}
}

TStatId USPShowTargetingUITask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPShowTargetingUITask, STATGROUP_USPAbility);
}

FString USPShowTargetingUITask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPShowTargetingUITask");
}

void USPShowTargetingUITask::StartTraceTargetActors(const UAbleAbilityContext* Context) const
{
	USPShowTargetingUITaskScratchPad* ScratchPad = Cast<USPShowTargetingUITaskScratchPad>(Context->GetScratchPadForTask(this));
	if (!ScratchPad)
	{
		return;
	}

	bool bActorTraced = false;
	if (TargetCount > 0)
	{
		for (int32 i = 0; i < TargetCount; ++i)
		{
			const AActor* CurrentTraceActor = ScratchPad->GetTraceActorByIndex(i);
			AActor* ResultActor = GetSingleActorFromTargetType(Context, EAbleAbilityTargetType::ATT_TargetActor, i);
			if (ResultActor)
			{
				bActorTraced = true;
				if (CurrentTraceActor != ResultActor)
				{
					ScratchPad->AddTraceActorByIndex(i, ResultActor);
					if (CurrentTraceActor)
					{
						UnTraceActor(CurrentTraceActor);
					}
					FSPShowTargetingUIData TraceData;
					if (GetTraceDataByIndex(i, TraceData))
					{
						if (TraceData.IsValidData())
						{
							TraceActor(ResultActor, TraceData);
						}
					}
				}
			}
			else
			{
				if (CurrentTraceActor)
				{
					UnTraceActor(CurrentTraceActor);
					ScratchPad->RemoveTraceActorByIndex(i);
				}
			}
		}
	}
	if (bActorTraced != ScratchPad->GetActorTraced())
	{
		ScratchPad->SetActorTraced(bActorTraced);
		SendTraceEvent(bActorTraced);
	}
}

void USPShowTargetingUITask::EndTraceTargetActors(const UAbleAbilityContext* Context) const
{
	USPShowTargetingUITaskScratchPad* ScratchPad = Cast<USPShowTargetingUITaskScratchPad>(Context->GetScratchPadForTask(this));
	if (!ScratchPad)
	{
		return;
	}

	for (int32 i = 0; i < TargetCount; ++i)
	{
		const AActor* TraceActor = ScratchPad->GetTraceActorByIndex(i);
		if (TraceActor)
		{
			UnTraceActor(TraceActor);
		}
	}
	
	ScratchPad->Reset();
}

bool USPShowTargetingUITask::GetTraceDataByIndex(const int32 Index, FSPShowTargetingUIData& OutUIData) const
{
	if (!TraceUIData.Num())
	{
		return false;
	}
	const int32 TargetIndex = FMath::Clamp(Index, 0, TraceUIData.Num() - 1);
	OutUIData = TraceUIData[TargetIndex];
	return true;
}

BEGIN_EXPORT_REFLECTED_CLASS(USPShowTargetingUITask)
ADD_FUNCTION(ShowTargetingUI)
ADD_FUNCTION(HideTargetingUI)
ADD_FUNCTION(TraceActor)
ADD_FUNCTION(UnTraceActor)
ADD_FUNCTION(SendTraceEvent)
END_EXPORT_CLASS(USPShowTargetingUITask)
IMPLEMENT_EXPORTED_CLASS(USPShowTargetingUITask)

#undef LOCTEXT_NAMESPACE

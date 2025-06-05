// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SPGame/Skill/Task/SPMonsterEmoteTask.h"
#include "Game/SPGame/Character/SPGameMonsterBase.h"
#include "Game/SPGame/Monster/SPGameMonsterFaceEmoteComponent.h"

USPMonsterEmoteTask::USPMonsterEmoteTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MouthEmoteType(ESP_PalMouthEmoteType::Empty)
	, EmoteType(ESP_PalEmoteType::Empty)
{
}

FString USPMonsterEmoteTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPMonsterEmoteTask");
}

void USPMonsterEmoteTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);
	OnTaskStartBP(Context.Get());
}

void USPMonsterEmoteTask::OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const
{

	const ASPGameMonsterBase* SPMonster = Cast<ASPGameMonsterBase>(Context->GetSelfActor());
	if (!IsValid(SPMonster))
		return;
	
	PlayFaceExpression(SPMonster);
}

TStatId USPMonsterEmoteTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPMonsterEmoteTask, STATGROUP_USPAbility);
}

void USPMonsterEmoteTask::PlayFaceExpression(const ASPGameMonsterBase* SPMonster) const
{
#if !UE_SERVER
	bool Founded = false;
	USkeletalMeshComponent* SkeletalMeshComponent = SPMonster->FindComponentByClass<USkeletalMeshComponent>();
	if (!IsValid(SkeletalMeshComponent))
		return;
	USPGameMonsterFaceEmoteComponent* FaceEmoteComponent = Cast<USPGameMonsterFaceEmoteComponent>(
			SPMonster->GetComponentByClass(USPGameMonsterFaceEmoteComponent::StaticClass()));
	if (FaceEmoteComponent)
	{
		TArray<FHeadEmoteForTask> PlayList;
		for (const FHeadEmoteForTask& Head : HeadList)
		{
			PlayList.Add(Head);
		}
		
		if (IsOnlyHead)
		{
			if (PlayList.Num() == 0)
			{
				PlayList.Add(FHeadEmoteForTask());
			}
			PlayList[0].HeadID = 0;
			PlayList[0].EyeEmote = EmoteType;
			PlayList[0].MouthEmote = MouthEmoteType;
		}

		if (IsMouthEmote)
		{
			TArray<FHeadMouthEmote> NextHeadEmoteList;
			TMap<int32, bool> DeadEmoteMap;
			TMap<int32, FHitEmote> HitEmoteMap;
			for (const auto& Emote : PlayList)
			{
				FHeadMouthEmote NextHeadEmote;
				NextHeadEmote.HeadID = Emote.HeadID;
				NextHeadEmote.MontageMouthEmote = Emote.MouthEmote;
				NextHeadEmoteList.Add(NextHeadEmote);
			}
			FaceEmoteComponent->PlayMouthEmote_FromMontage(NextHeadEmoteList, DeadEmoteMap, HitEmoteMap);
		}
		else
		{
			TArray<FHeadEyeEmote> NextHeadEmoteList;
			TMap<int32, bool> DeadEmoteMap;
			TMap<int32, FHitEmote> HitEmoteMap;
			for (const auto& Emote : PlayList)
			{
				FHeadEyeEmote NextHeadEmote;
				NextHeadEmote.HeadID = Emote.HeadID;
				NextHeadEmote.MontageEyeEmote = Emote.EyeEmote;
				NextHeadEmoteList.Add(NextHeadEmote);
			}
			FaceEmoteComponent->PlayFaceEmote_FromMontage(NextHeadEmoteList, DeadEmoteMap, HitEmoteMap);
		}

		Founded = true;
	}

	if (!Founded)
	{
		if (IsOnlyHead)
		{
			UMaterialInterface* MaterialInstance = SkeletalMeshComponent->GetMaterial(EmoteMaterialIndex);
			if (MaterialInstance)
			{
				UMaterialInstanceDynamic* MaterialInstanceDynamic = SkeletalMeshComponent->CreateDynamicMaterialInstance(EmoteMaterialIndex, MaterialInstance);
				if (MaterialInstanceDynamic)
				{
					MaterialInstanceDynamic->SetScalarParameterValue("Index", EmoteIndex);
				}
			}
		}
		else
		{
			int32 Index = 0;
			for (const auto& MatIndex : EmoteMaterialIndexList)
			{
				if (Index >= HeadList.Num())
				{
					break;
				}
				UMaterialInterface* MaterialInstance = SkeletalMeshComponent->GetMaterial(MatIndex);
				if (MaterialInstance)
				{
					UMaterialInstanceDynamic* MaterialInstanceDynamic = SkeletalMeshComponent->CreateDynamicMaterialInstance(MatIndex, MaterialInstance);
					if (MaterialInstanceDynamic)
					{
						MaterialInstanceDynamic->SetScalarParameterValue("Index", IsMouthEmote ? static_cast<int32>(HeadList[Index].MouthEmote) : static_cast<int32>(HeadList[Index].EyeEmote));
					}
				}
				Index++;
			}
		}
	}
#endif
}

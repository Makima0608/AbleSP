// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UnLuaInterface.h"
#include "CoreMinimal.h"
#include "Tasks/IAbleAbilityTask.h"
#include "Game/SPGame/Character/SP_PalEmoteType.h"
#include "SPMonsterEmoteTask.generated.h"

#define LOCTEXT_NAMESPACE "USPMonsterEmoteTask"

class ASPGameMonsterBase;

USTRUCT(BlueprintType)
struct FHeadEmoteForTask
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 HeadID;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "!IsMouthEmote"))
	ESP_PalEmoteType EyeEmote;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "IsMouthEmote"))
	ESP_PalMouthEmoteType MouthEmote;
};

/**
 * 
 */
UCLASS()
class FEATURE_SP_API USPMonsterEmoteTask : public UAbleAbilityTask, public IUnLuaInterface
{
	GENERATED_BODY()

protected:
	virtual FString GetModuleName_Implementation() const override;
protected:
	USPMonsterEmoteTask(const FObjectInitializer& ObjectInitializer);

public:
	/* Called as soon as the task is started. Do any per-run initialization here.*/
	virtual void OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskStart"))
	void OnTaskStartBP(const UAbleAbilityContext* Context) const;

	/* Returns whether this Task only occurs during a Single Frame, or not. */
	FORCEINLINE virtual bool  IsSingleFrame() const { return true; }

	/* Returns the StatId for this Task, used by the Profiler. */
	virtual TStatId GetStatId() const;
	
#if WITH_EDITOR
	virtual bool NeedStartInPreviewTimeSet(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override { return false; }
	
	virtual FText GetTaskCategory() const override { return LOCTEXT("USPMonsterEmoteTask", "Monster"); }
	
	virtual FText GetTaskName() const override { return LOCTEXT("USPMonsterEmoteTask", "Emote"); }
	
	virtual FText GetTaskDescription() const override { return LOCTEXT("USPMonsterEmoteTask", "Change Monster Emote"); }
	
	virtual FLinearColor GetTaskColor() const override { return FLinearColor(67.0f / 255.0f, 110.0f / 255.0f, 238.0f / 255.0f); }
	
	virtual bool CanEditTaskRealm() const override { return false; }
#endif

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "√ 嘴部 X 脸部"))
	bool IsMouthEmote = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsOnlyHead = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "嘴或脸材质ID", EditCondition = "IsOnlyHead"))
	int32 EmoteMaterialIndex = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "嘴或脸材质IDList", EditCondition = "!IsOnlyHead"))
	TArray<int32> EmoteMaterialIndexList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "嘴类型", EditCondition = "IsMouthEmote"))
	ESP_PalMouthEmoteType MouthEmoteType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "脸类型", EditCondition = "!IsMouthEmote"))
	ESP_PalEmoteType EmoteType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (DisplayName = "表情ID"))
	int32 EmoteIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "头List", EditCondition = "!IsOnlyHead"))
	TArray<FHeadEmoteForTask> HeadList;
private:
	void PlayFaceExpression(const ASPGameMonsterBase* SPMonster) const;
};

#undef LOCTEXT_NAMESPACE
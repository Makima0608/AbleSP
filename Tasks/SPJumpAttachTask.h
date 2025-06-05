// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UnLuaInterface.h"
#include "CoreMinimal.h"
#include "Stats/Stats2.h"
#include "Engine/EngineTypes.h"
#include "Curves/RichCurve.h"

#include "Tasks/SPAbilityTask.h"

#include "SPJumpAttachTask.generated.h"



class USPJumpAttachFlowLogic;
class USPJumpAttachFlowData;
struct FSPJumpAttachFlowParams;
class USPJumpAttachConfig;


#define LOCTEXT_NAMESPACE "SPAbleAbilityTask"

/* Scratchpad for our Task. */
UCLASS(Abstract, Transient, Blueprintable)
class FEATURE_SP_API USPJumpAttachTaskScratchPad : public USPAbilityTaskScratchPad
{
	GENERATED_BODY()
public:
	USPJumpAttachTaskScratchPad();

	UPROPERTY(BlueprintReadWrite)
	USPJumpAttachFlowLogic* FlowLogic;

	UPROPERTY(BlueprintReadWrite)
	USPJumpAttachFlowData* FlowData;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ResetPad();

	bool HasFlowStarted() const;
};


UCLASS(Abstract, Blueprintable)
class FEATURE_SP_API USPJumpAttachTask : public USPAbilityTask
{
	GENERATED_BODY()

protected:
	virtual FString GetModuleName_Implementation() const override;

// ~UAbleAbilityTask begin
protected:
	virtual void OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const override;
	virtual void OnTaskTickBP_Implementation(const UAbleAbilityContext* Context, float DeltaTime) const override;
	virtual void OnTaskEndBP_Implementation(const UAbleAbilityContext* Context, const EAbleAbilityTaskResult result) const override;
	virtual bool IsAsyncFriendly() const override  { return false; }
	virtual bool IsSingleFrameBP_Implementation() const override { return false; }
	virtual bool IsDoneBP_Implementation(const UAbleAbilityContext* Context) const override;
	virtual EAbleAbilityTaskRealm GetTaskRealmBP_Implementation() const override { return EAbleAbilityTaskRealm::ATR_Client; } // Client for Auth client, Server for AIs/Proxies.
	virtual USPAbilityTaskScratchPad* CreateScratchPadBP_Implementation(UAbleAbilityContext* Context) const override;
	virtual TStatId GetStatId() const override;

#if WITH_EDITOR
	virtual FText GetTaskCategoryBP_Implementation() const override { return LOCTEXT("SPJumpAttachCategory", "Movement"); }
	virtual FText GetTaskNameBP_Implementation() const override { return LOCTEXT("SPJumpAttachTask", "Jump and attach"); }
	virtual FText GetDescriptiveTaskNameBP_Implementation() const override;
	virtual FText GetTaskDescriptionBP_Implementation() const override { return LOCTEXT("SPJumpAttachTaskDesc", "Jump and attach"); }
	virtual FLinearColor GetTaskColorBP_Implementation() const override { return FLinearColor(83.0f / 255.0f, 214.0f / 255.0f, 93.0f / 255.0f); }
#endif
// ~UAbleAbilityTask end

public:
	USPJumpAttachTask(const FObjectInitializer& ObjectInitializer);
private:
	bool CreateFlowParams(
		FSPJumpAttachFlowParams& OutParams,
		USPAbilityTaskScratchPad& IoScratchPadRef,
		const UAbleAbilityContext& InContextRef) const;
protected:
	/* parameter name in ability context to get the moved component. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ParamName", meta = (DisplayName = "JumpMovedCompParamName"))
	FName m_JumpMovedCompParamName;

	/* parameter name in ability context to get the move space component. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ParamName", meta = (DisplayName = "JumpSpaceCompParamName"))
	FName m_JumpSpaceCompParamName;

	/* parameter name in ability context to get the target parent component. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ParamName", meta = (DisplayName = "JumpTargetParentCompParamName"))
	FName m_JumpTargetParentCompParamName;

	/* parameter name in ability context to get the target parent component. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ParamName", meta = (DisplayName = "JumpMoveRootCompParamName"))
	FName m_JumpMoveRootCompParamName;

	/* parameter name in ability context to get socket name. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ParamName", meta = (DisplayName = "JumpAttachSocketNameParamName"))
	FName m_JumpAttachSocketNameParamName;

	/* parameter name in ability context to get jump start location in move space. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ParamName", meta = (DisplayName = "JumpStartLocInJumpSpaceParamName"))
	FName m_JumpStartLocInJumpSpaceParamName;

	/* parameter name in ability context to get jump start rotation in move space. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ParamName", meta = (DisplayName = "JumpStartRotInJumpSpaceParamName"))
	FName m_JumpStartRotInJumpSpaceParamName;

	/* parameter name in ability context to get jump end location in move space. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ParamName", meta = (DisplayName = "JumpEndLocInJumpSpaceParamName"))
	FName m_JumpEndLocInJumpSpaceParamName;

	/* parameter name in ability context to get jump end rotation in move space. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ParamName", meta = (DisplayName = "JumpEndRotInJumpSpaceParamName"))
	FName m_JumpEndRotInJumpSpaceParamName;

	/* parameter name in ability context to get jump animation, anim motage or anim sequence. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ParamName", meta = (DisplayName = "JumpAnimParamName"))
	FName m_JumpAnimParamName;

	/* parameter name in ability context to get attach offset location. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ParamName", meta = (DisplayName = "JumpAttachOffsetLocParamName"))
	FName m_JumpAttachOffsetLocParamName;

	/* parameter name in ability context to get task duration. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ParamName", meta = (DisplayName = "JumpDurationParamName"))
	FName m_JumpDurationParamName;

	/* parameter name in ability context to get anim play rate. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ParamName", meta = (DisplayName = "JumpAnimPlayRateParamName"))
	FName m_JumpAnimPlayRateParamName;

	/* parameter name in ability context to get jump peak z in jump space. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ParamName", meta = (DisplayName = "JumpPeakZInJumpSpaceParamName"))
	FName m_JumpPeakZInJumpSpaceParamName;

	/* parameter name in ability context to get jump attach config. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ParamName", meta = (DisplayName = "JumpAttachConfigParamName"))
	FName m_JumpAttachConfigParamName;

	/* parameter name in ability context to get jump attach anim meta data. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ParamName", meta = (DisplayName = "JumpAttachAnimMetaDataParamName"))
	FName m_JumpAttachAnimMetaDataParamName;

	/* parameter name in ability context to get jump attach flow logic */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ParamName", meta = (DisplayName = "JumpAttachFlowLogicParamName"))
	FName m_JumpAttachFlowLogicParamName;

	/* parameter name in ability context to get jump attach flow data */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ParamName", meta = (DisplayName = "JumpAttachFlowDataParamName"))
	FName m_JumpAttachFlowDataParamName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (DisplayName = "JumpAttachConfig"))
	USPJumpAttachConfig* m_JumpAttachConfig;
};

#undef LOCTEXT_NAMESPACE

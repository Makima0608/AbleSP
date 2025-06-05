// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UnLuaInterface.h"
#include "CoreMinimal.h"
#include "Stats/Stats2.h"
#include "Engine/EngineTypes.h"

#include "Tasks/SPAbilityTask.h"

#include "Game/SPGame/Common/JumpAttach/SPJumpAttachTypes.h"

#include "SPJumpAttachRootMotionTask.generated.h"



class UScriptStruct;
class UCurveVector;
class UCurveFloat;
class UPrimitiveComponent;
class USceneComponent;
class UCharacterMovementComponent;
class ACharacter;
class UScriptStruct;


#define LOCTEXT_NAMESPACE "SPAbleAbilityTask"


/* Scratchpad for our Task. */
UCLASS(Abstract, Transient, Blueprintable)
class FEATURE_SP_API USPJumpAttachRootMotionTaskScratchPad : public USPAbilityTaskScratchPad
{
	GENERATED_BODY()
public:
	USPJumpAttachRootMotionTaskScratchPad();

	UPROPERTY()
	uint16 RootMotionSourceID;

	UPROPERTY(BlueprintReadWrite)
	bool bAttachLerping;

	UPROPERTY(BlueprintReadWrite)
	bool bRotationLerping;

	UPROPERTY(BlueprintReadWrite)
	ENetworkSmoothingMode OldNetSmoothMode;

	UPROPERTY(BlueprintReadWrite)
	float JumpPeakTime;

	UPROPERTY(BlueprintReadWrite)
	FVector DefaultOwnerMeshLoc;

	UPROPERTY(BlueprintReadWrite)
	FVector LerpEndOwnerMeshLoc;

	UPROPERTY(BlueprintReadWrite)
	FQuat DefaultOwnerMeshQuat;

	UPROPERTY(BlueprintReadWrite)
	FQuat LerpEndOwnerMeshQuat;

	UPROPERTY(BlueprintReadWrite)
	FQuat OwnerJumpStartRotationInTargetRootSpace;

	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<UPrimitiveComponent> TargetMeshCompPtr;

	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<USceneComponent> OldMeshParentCompPtr;

	UPROPERTY(BlueprintReadWrite)
	FName SocketName;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ResetPad();
};


UCLASS(Abstract, Blueprintable)
class FEATURE_SP_API USPJumpAttachRootMotionTask : public USPAbilityTask
{
	GENERATED_BODY()

protected:
	virtual FString GetModuleName_Implementation() const override;

// ~UAbleAbilityTask begin
protected:
	virtual void OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const override;
	virtual void OnTaskTickBP_Implementation(const UAbleAbilityContext* Context, float DeltaTime) const override;
	virtual void OnTaskEndBP_Implementation(const UAbleAbilityContext* Context, const EAbleAbilityTaskResult result) const override;
	virtual bool IsAsyncFriendly() const { return false; }
	virtual bool IsSingleFrameBP_Implementation() const { return false; }
	virtual EAbleAbilityTaskRealm GetTaskRealmBP_Implementation() const override { return EAbleAbilityTaskRealm::ATR_ClientAndServer; } // Client for Auth client, Server for AIs/Proxies.
	virtual USPAbilityTaskScratchPad* CreateScratchPadBP_Implementation(UAbleAbilityContext* Context) const override;
	virtual TStatId GetStatId() const override;

#if WITH_EDITOR
	virtual FText GetTaskCategoryBP_Implementation() const override { return LOCTEXT("SPJumpAttachRootMotionCategory", "RootMotionSource"); }
	virtual FText GetTaskNameBP_Implementation() const override { return LOCTEXT("SPJumpAttachRootMotionTask", "Jump and attach"); }
	virtual FText GetDescriptiveTaskNameBP_Implementation() const override;
	virtual FText GetTaskDescriptionBP_Implementation() const override { return LOCTEXT("SPJumpAttachRootMotionTaskDesc", "Jump and attach with root motion source"); }
	virtual FLinearColor GetTaskColorBP_Implementation() const override { return FLinearColor(83.0f / 255.0f, 214.0f / 255.0f, 93.0f / 255.0f); }
#endif
// ~UAbleAbilityTask end

public:
	USPJumpAttachRootMotionTask(const FObjectInitializer& ObjectInitializer);
protected:
	static FVector CalculateOwnerAttachLocationOffset(
		const UPrimitiveComponent& InOwnerMoveUpdateCompRef,
		const FVector& InCollisionExtentAttachFactor,
		FVector* IoOwnerCollisionExtentPtr = nullptr);
protected:
	/* when owner is attach to target mesh socket, for example, (0,0,-1) means owner capsule will offset half of capsule height. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Jump", meta = (DisplayName = "CollisionSizeAttachFactor"))
	FVector m_CollisionExtentAttachFactor;

	/* parameter name in ability context to get the target scene component. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ParamName", meta = (DisplayName = "TargetCompParamName"))
	FName m_TargetCompParamName;

	/* parameter name in ability context to get socket name. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ParamName", meta = (DisplayName = "SocketNameParamName"))
	FName m_SocketNameParamName;

	/* parameter name in ability context to get start location in target actor space. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ParamName", meta = (DisplayName = "StartLocInTargetActorSpaceParamName"))
	FName m_StartLocInTargetActorSpaceParamName;

	/* parameter name in ability context to get attach location in target actor space. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ParamName", meta = (DisplayName = "AttachLocInTargetActorSpaceParamName"))
	FName m_AttachLocInTargetActorSpaceParamName;

	/* z scalar accumlated by target collision size. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Jump", meta = (DisplayName = "TargetHeightFactor"))
	float m_TargetHeightFactor;

	/* base z offset at attach point. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Jump", meta = (DisplayName = "TargetHeightOffset"))
	float m_TargetHeightOffset;

	/* when target below floor. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Jump", meta = (DisplayName = "MinJumpHeight"))
	float m_MinJumpHeight;

	/* the socket name to attach. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attach", meta = (DisplayName = "SocketName"))
	FName m_SocketName;

	/* After jump peak time, when owner enter range at attachpoint, start attach lerp effect. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attach", meta = (DisplayName = "AttachRadiusSquared"))
	float m_AttachRadiusSquared;

	/* actor rotation start time, relative to task. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Jump", meta = (DisplayName = "StartRotationTime"))
	float m_StartRotationTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Curves", meta = (DisplayName = "PathOffsetCurve"))
	TSoftObjectPtr<UCurveVector> m_PathOffsetCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Curves", meta = (DisplayName = "TimeMappingCurve"))
	TSoftObjectPtr<UCurveFloat> m_TimeMappingCurve;

	/*x is [0, 1], y is attach lerping percent*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Curves", meta = (DisplayName = "AttachLerpingCurve"))
	TSoftObjectPtr<UCurveFloat> m_AttachLerpingCurve;
};

#undef LOCTEXT_NAMESPACE

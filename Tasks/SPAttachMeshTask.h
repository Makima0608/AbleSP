// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnLuaInterface.h"
#include "Tasks/IAbleAbilityTask.h"
#include "SPAttachMeshTask.generated.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

UENUM(BlueprintType)
enum EMeshAttachRule
{
	AttachStaticMesh,
	AttachSkeletalMesh,
};

UCLASS(Transient)
class USPAttachMeshTaskScratchPad : public UAbleAbilityTaskScratchPad
{
	GENERATED_BODY()

public:
	USPAttachMeshTaskScratchPad();

	virtual ~USPAttachMeshTaskScratchPad();

	UPROPERTY()
	UMeshComponent* AttachComponent = nullptr;
	
};

/**
 * 
 */
UCLASS()
class FEATURE_SP_API USPAttachMeshTask : public UAbleAbilityTask, public IUnLuaInterface
{
	GENERATED_BODY()
protected:
	virtual FString GetModuleName_Implementation() const override;

public:
	virtual void ResetScratchPad(UAbleAbilityTaskScratchPad* ScratchPad) const override;
	
	virtual void OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskStart"))
	void OnTaskStartBP(const UAbleAbilityContext* Context) const;
	
	virtual void OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context, const EAbleAbilityTaskResult result) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskEnd"))
	void OnTaskEndBP(const UAbleAbilityContext* Context,
					 const EAbleAbilityTaskResult result) const;

	UMeshComponent* CreateMeshComponentToAttach(AActor* AttachActor, const USkeletalMeshComponent* SkeletonMeshComponent) const;

	virtual UAbleAbilityTaskScratchPad* CreateScratchPad(const TWeakObjectPtr<UAbleAbilityContext>& Context) const override;
	
	virtual TStatId GetStatId() const override;

#if WITH_EDITOR

	virtual FText GetTaskCategory() const override { return LOCTEXT("USPAttachMeshTask", "GamePlay"); }

	virtual FText GetTaskName() const override { return LOCTEXT("USPAttachMeshTask", "Attach Mesh"); }

	virtual bool CanEditTaskRealm() const override { return true; }
#endif

protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Attach", meta = (DisplayName = "IsDeAttach"))
	bool m_IsDeAttach;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Attach", meta = (DisplayName = "IsRecoverOnEnd"))
	bool m_IsRecoverOnEnd;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Attach", meta = (DisplayName = "Attach Target"))
	TEnumAsByte<EAbleAbilityTargetType> m_Target;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Attach", meta = (DisplayName = "Attach Rule"))
	TEnumAsByte<EMeshAttachRule> m_AttachRule;

	UPROPERTY(EditAnywhere, Category = "Attach",
		meta = (DisplayName = "Attach StaticMesh", EditCondition = "m_AttachRule==0"))
	TSoftObjectPtr<UStaticMesh> m_DecorationMesh = nullptr;

	UPROPERTY(EditAnywhere, Category = "Attach",
		meta = (DisplayName = "Attach SkeletalMesh", EditCondition = "m_AttachRule==1"))
	TSoftObjectPtr<USkeletalMesh> m_DecorationSkeletalMesh = nullptr;

	UPROPERTY(EditAnywhere, Category = "Attach", meta = (DisplayName = "Attach Socket"))
	FName m_Socket;

	UPROPERTY(EditAnywhere, Category = "Attach", meta = (DisplayName = "Attach Relative Transform"))
	FTransform m_DecorationRelativeTransform;
	
};

#undef LOCTEXT_NAMESPACE

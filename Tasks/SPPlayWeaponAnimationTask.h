// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnLuaInterface.h"
#include "Tasks/IAbleAbilityTask.h"
#include "SPPlayWeaponAnimationTask.generated.h"

#define LOCTEXT_NAMESPACE "AblAbilityTask"

class ASPWeapon;

UCLASS(Transient)
class FEATURE_SP_API USPPlayWeaponAnimationTaskScratchPad : public UAbleAbilityTaskScratchPad
{
	GENERATED_BODY()

public:
	USPPlayWeaponAnimationTaskScratchPad();
	virtual ~USPPlayWeaponAnimationTaskScratchPad() override;

	/* The Skeletal Mesh Components of all the actors we affected (Single Node only). */
	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<USkeletalMeshComponent>> SingleNodeSkeletalComponents;
};

/**
 * 
 */
UCLASS()
class FEATURE_SP_API USPPlayWeaponAnimationTask : public UAbleAbilityTask, public IUnLuaInterface
{
	GENERATED_BODY()

protected:
	virtual FString GetModuleName_Implementation() const override;

public:
	USPPlayWeaponAnimationTask(const FObjectInitializer& ObjectInitializer);
	virtual ~USPPlayWeaponAnimationTask() override;

	virtual void OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskStart"))
	void OnTaskStartBP(const UAbleAbilityContext* Context) const;

	virtual void OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context, const EAbleAbilityTaskResult Result) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskEnd"))
	void OnTaskEndBP(const UAbleAbilityContext* Context, const EAbleAbilityTaskResult Result) const;

	virtual TStatId GetStatId() const override;

	virtual UAbleAbilityTaskScratchPad* CreateScratchPad(const TWeakObjectPtr<UAbleAbilityContext>& Context) const override;
	
#if WITH_EDITOR
	virtual FText GetTaskName() const override { return LOCTEXT("SPPlayWeaponAnimationTask", "Play Weapon Animation"); }
	virtual FText GetTaskCategory() const override { return LOCTEXT("SPPlayWeaponAnimationTask", "Animation"); }
	virtual FText GetDescriptiveTaskName() const override;
	virtual FLinearColor GetTaskColor() const override { return FLinearColor(155.0f / 255.0f, 44.0f / 255.0f, 87.0f / 255.0f); }
#endif

protected:
	void GetWeaponActors(const TWeakObjectPtr<AActor> Owner, TArray<TWeakObjectPtr<ASPWeapon>>& OutWeaponArray) const;

protected:
	UPROPERTY(EditAnywhere, Category="Animation", meta = (DisplayName = "Animation", AllowedClasses = "AnimMontage,AnimSequence"))
	TSoftObjectPtr<UAnimSequence> AnimSequence = nullptr;

	UPROPERTY(EditAnywhere, Category = "Animation", meta = (DisplayName = "Slot Name"))
	FName SlotName = TEXT("DefaultSlot");

	UPROPERTY(EditAnywhere, Category = "Animation", meta=(ToolTip="可指定武器Actor名称关键字"))
	FString WeaponName = "";

	UPROPERTY(EditAnywhere, Category = "Animation", meta = (DisplayName = "Play Rate", EditCondition = "AnimationAsset!=nullptr"))
	float PlayRate = 1.0f;
	
	UPROPERTY(EditAnywhere, Category = "Animation")
    float StartTime = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Animation", meta = (DisplayName = "Loop"))
	bool bIsLoop = false;

	UPROPERTY(EditAnywhere, Category = "Animation", meta = (DisplayName = "Number Of Loops"))
	int32 NumberOfLoops = 1;

	UPROPERTY(EditAnywhere, Category = "Animation", meta = (DisplayName = "Stop Animation On Interrupt"))
	bool bStopAtInterrupt = true;
	
	UPROPERTY(EditAnywhere, Category = "Animation", meta = (DisplayName = "Stop Animation On BranchSegment"))
    bool bStopAtBranch = false;

	UPROPERTY(EditAnywhere, Category = "Animation")
	float BlendOutTime = 0.2f;

	UPROPERTY(EditAnywhere, Category = "Animation")
	float BlendInTime = 0.2f;
};

#undef LOCTEXT_NAMESPACE
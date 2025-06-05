// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UnLuaInterface.h"
#include "CoreMinimal.h"
#include "MoeGameplay/Weapon/Weapon.h"
#include "Tasks/IAbleAbilityTask.h"
#include "SPAttachWeaponTask.generated.h"

#define LOCTEXT_NAMESPACE "SPAbilityTask"

UCLASS(Transient)
class USAttachWeaponTaskScratchPad : public UAbleAbilityTaskScratchPad
{
	GENERATED_BODY()

public:
	USAttachWeaponTaskScratchPad();

	virtual ~USAttachWeaponTaskScratchPad();

	UPROPERTY()
	AActor* CacheWeapon = nullptr;
};

/**
 *
 */
UCLASS()
class FEATURE_SP_API USPAttachWeaponTask : public UAbleAbilityTask, public IUnLuaInterface
{
	GENERATED_BODY()

protected:
	virtual FString GetModuleName_Implementation() const override;

	USPAttachWeaponTask(const FObjectInitializer& ObjectInitializer);

	virtual ~USPAttachWeaponTask();

	virtual void OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskStart"))
	void OnTaskStartBP(const UAbleAbilityContext* Context) const;

	virtual void OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context, float deltaTime) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskTick"))
	void OnTaskTickBP(const UAbleAbilityContext* Context, float deltaTime) const;

	virtual void OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	                       const EAbleAbilityTaskResult result) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskEnd"))
	void OnTaskEndBP(const UAbleAbilityContext* Context,
					 const EAbleAbilityTaskResult result) const;

	AActor* CreateWeaponToAttach(AActor* AttachActor, const USkeletalMeshComponent* SkeletonMeshComponent) const;

	AActor* CreatDefaultToAttach(AActor* AttachActor, const USkeletalMeshComponent* SkeletonMeshComponent) const;

	virtual bool IsSingleFrame() const override { return IsSingleFrameBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "IsSingleFrame"))
	bool IsSingleFrameBP() const;

	virtual UAbleAbilityTaskScratchPad*
	CreateScratchPad(const TWeakObjectPtr<UAbleAbilityContext>& Context) const override;

	virtual TStatId GetStatId() const override;

#if WITH_EDITOR

	virtual FText GetTaskCategory() const override { return LOCTEXT("USPAttachWeaponTask", "Misc"); }

	virtual FText GetTaskName() const override { return LOCTEXT("USPAttachWeaponTask", "Attach Weapon"); }

	virtual bool CanEditTaskRealm() const override { return true; }
#endif

protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Weapon", meta = (DisplayName = "Attach Target"))
	TEnumAsByte<EAbleAbilityTargetType> m_Target;

	UPROPERTY(EditAnywhere, Category = "Weapon",
		meta = (DisplayName = "Preview Weapon"))
	TSubclassOf<AWeapon> m_PreviewWeapon;

	UPROPERTY(EditAnywhere, Category = "Weapon", meta = (DisplayName = "Attach Socket"))
	FName m_Socket;

	UPROPERTY(EditAnywhere, Category = "Weapon", meta = (DisplayName = "Attach Relative Transform"))
	FTransform m_RelativeTransform;

	UPROPERTY(EditAnywhere, Category = "Weapon", meta = (DisplayName = "Hide Weapon Mesh"))
	bool bHideWeaponMesh;
};

#undef LOCTEXT_NAMESPACE

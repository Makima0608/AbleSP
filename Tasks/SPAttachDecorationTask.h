// Copyright (c) Extra Life Studios, LLC. All rights reserved.

#pragma once

#include "UnLuaInterface.h"
#include "CoreMinimal.h"
#include "Tasks/IAbleAbilityTask.h"
#include "SPAttachDecorationTask.generated.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

UENUM(BlueprintType)
enum EDecorationAttachRule
{
	StaticMesh,
	SkeletalMesh,
};

UCLASS(Transient)
class USAttachDecoratorTaskScratchPad : public UAbleAbilityTaskScratchPad
{
	GENERATED_BODY()

public:
	USAttachDecoratorTaskScratchPad();

	virtual ~USAttachDecoratorTaskScratchPad();

	UPROPERTY()
	UMeshComponent* DecorationComponent = nullptr;

	UPROPERTY()
	AActor* DecorationActor = nullptr;
};

/**
 * 
 */
UCLASS()
class FEATURE_SP_API USPAttachDecorationTask : public UAbleAbilityTask, public IUnLuaInterface
{
	GENERATED_BODY()

protected:
	virtual FString GetModuleName_Implementation() const override;

public:
	USPAttachDecorationTask(const FObjectInitializer& ObjectInitializer);

	virtual ~USPAttachDecorationTask();

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

	virtual bool IsSingleFrame() const override { return IsSingleFrameBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "IsSingleFrame"))
	bool IsSingleFrameBP() const;

	UMeshComponent* CreateMeshComponentToAttach(AActor* AttachActor, const USkeletalMeshComponent* SkeletonMeshComponent) const;

	virtual UAbleAbilityTaskScratchPad*
	CreateScratchPad(const TWeakObjectPtr<UAbleAbilityContext>& Context) const override;

	virtual TStatId GetStatId() const override;

#if WITH_EDITOR

	virtual FText GetTaskCategory() const override { return LOCTEXT("USPAttachDecorationTask", "Misc"); }

	virtual FText GetTaskName() const override { return LOCTEXT("USPAttachDecorationTask", "Attach Decoration"); }

	virtual bool CanEditTaskRealm() const override { return true; }
#endif

protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Decoration", meta = (DisplayName = "Decorate Target"))
	TEnumAsByte<EAbleAbilityTargetType> m_Target;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Decoration", meta = (DisplayName = "Decorate Rule"))
	TEnumAsByte<EDecorationAttachRule> m_AttachRule;

	UPROPERTY(EditAnywhere, Category = "Decoration",
		meta = (DisplayName = "Decoration StaticMesh", EditCondition = "m_AttachRule==0"))
	TSoftObjectPtr<UStaticMesh> m_DecorationMesh = nullptr;

	UPROPERTY(EditAnywhere, Category = "Decoration",
		meta = (DisplayName = "Decoration SkeletalMesh", EditCondition = "m_AttachRule==1"))
	TSoftObjectPtr<USkeletalMesh> m_DecorationSkeletalMesh = nullptr;

	UPROPERTY(EditAnywhere, Category = "Decoration", meta = (DisplayName = "Decoration Socket"))
	FName m_Socket;

	UPROPERTY(EditAnywhere, Category = "Decoration", meta = (DisplayName = "Decoration Relative Transform"))
	FTransform m_DecorationRelativeTransform;

	UPROPERTY(EditAnywhere, Category = "Decoration",
		meta = (DisplayName = "Animation to Play On Start", EditCondition = "m_DecorationSkeletalMesh!=nullptr"))
	TSoftObjectPtr<UAnimationAsset> m_AnimationMontageOnstart;

	UPROPERTY(EditAnywhere, Category = "Decoration",
		meta = (DisplayName = "Loop", EditCondition = "m_DecorationSkeletalMesh!=nullptr"))
	bool bIsLoop = false;
};

#undef LOCTEXT_NAMESPACE

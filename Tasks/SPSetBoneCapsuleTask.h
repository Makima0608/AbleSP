// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tasks/IAbleAbilityTask.h"
#include "SPSetBoneCapsuleTask.generated.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"
/**
 * 
 */
UCLASS()
class FEATURE_SP_API USPSetBoneCapsuleTask : public UAbleAbilityTask
{
	GENERATED_BODY()
public:
	virtual TStatId GetStatId() const override;
	virtual void OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override;

	virtual void OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context, float deltaTime) const override;

	virtual void OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context, const EAbleAbilityTaskResult result) const override;

#if WITH_EDITOR
	virtual FText GetTaskName() const { return LOCTEXT("SPSetBoneCapsuleTask", "SP SetBoneCapsule"); }

#endif

	virtual EAbleAbilityTaskRealm GetTaskRealm() const override { return GetTaskRealmBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "GetTaskRealm"))
	EAbleAbilityTaskRealm GetTaskRealmBP() const;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Bone", meta=(DisplayName="隐藏骨骼true还是显示骨骼false"))
	bool bIsHideBone = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Bone", meta=(DisplayName="骨骼节点名字"))
	TArray<FName> BonesName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Bone", meta=(DisplayName="是否设置子骨骼"))
	bool IsChangeBoneChildren = true;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Bone", meta=(DisplayName="task结束是否还原(待完善)"))
	bool bIsRecoverWhenEnd = false;
	
};

#undef LOCTEXT_NAMESPACE

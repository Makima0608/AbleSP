// Copyright (c) Extra Life Studios, LLC. All rights reserved.

#pragma once

#include "UnLuaInterface.h"
#include "CoreMinimal.h"
#include "Tasks/IAbleAbilityTask.h"
#include "Game/SPGame/Utils/SPGameLibrary.h"
#include "SPBroadcastAttackNotifyTask.generated.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

UCLASS()
class FEATURE_SP_API USPBroadcastAttackNotifyTask : public UAbleAbilityTask, public IUnLuaInterface
{
	GENERATED_BODY()

protected:
	virtual FString GetModuleName_Implementation() const override;

public:
	USPBroadcastAttackNotifyTask(const FObjectInitializer& Initializer);

	virtual ~USPBroadcastAttackNotifyTask();

	virtual void OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskStart"))
	void OnTaskStartBP(const UAbleAbilityContext* Context) const;

	virtual void OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	                       const EAbleAbilityTaskResult result) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskEnd"))
	void OnTaskEndBP(const UAbleAbilityContext* Context,
					 const EAbleAbilityTaskResult result) const;

	virtual bool IsSingleFrame() const override { return IsSingleFrameBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "IsSingleFrame"))
	bool IsSingleFrameBP() const;

	virtual void BindDynamicDelegates(UAbleAbility* Ability) override;
	
	virtual EAbleAbilityTaskRealm GetTaskRealm() const override { return GetTaskRealmBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "GetTaskRealm"))
	EAbleAbilityTaskRealm GetTaskRealmBP() const;

	virtual TStatId GetStatId() const override;

#if WITH_EDITOR
	virtual FText GetTaskCategory() const override { return LOCTEXT("USPBroadcastAttackNotifyTask", "Misc"); }

	virtual FText GetTaskName() const override { return LOCTEXT("USPBroadcastAttackNotifyTask", "Broadcast Attack Notify"); }
#endif

protected:
	UPROPERTY(EditAnywhere, Category = "Context", meta = (DisplayName = "Range"))
	float m_Range;

	UPROPERTY(EditAnywhere, Category = "Context", meta = (DisplayName = "Angle"))
	float m_Angle;

	UPROPERTY(EditAnywhere, Category = "Context", meta = (DisplayName = "Direction"))
	ESPBroadcastAttackNotifyDirection m_Direction;
};

#undef LOCTEXT_NAMESPACE

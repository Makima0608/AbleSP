#pragma once

#include "UnLuaInterface.h"
#include "CoreMinimal.h"
#include "Tasks/ablePlayParticleEffectTask.h"
#include "Tasks/IAbleAbilityTask.h"
#include "ableAbility.h"
#include "SPPlayParticleEffectTask.generated.h"


#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

UCLASS(Transient)
class FEATURE_SP_API USPPlayParticleEffectTaskScratchPad: public UAblePlayParticleEffectTaskScratchPad
{
	GENERATED_BODY()
	
public:
	USPPlayParticleEffectTaskScratchPad();
	virtual ~USPPlayParticleEffectTaskScratchPad();

	UPROPERTY(Transient)
	TMap<FString, int32> m_EffectsAndPlayingID;
};

/**
 *
 */
UCLASS()
class FEATURE_SP_API USPPlayParticleEffectTask : public UAblePlayParticleEffectTask
{
	GENERATED_BODY()

protected:
	virtual FString GetModuleName_Implementation() const override;

public:
	USPPlayParticleEffectTask(const FObjectInitializer& ObjectInitializer);
	virtual ~USPPlayParticleEffectTask();

	virtual void OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override;
	UFUNCTION(BlueprintNativeEvent)
	void OnTaskStartBP_Override(const UAbleAbilityContext* Context) const;

	virtual void OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context, float deltaTime) const override;
	UFUNCTION(BlueprintNativeEvent)
	void OnTaskTickBP_Override(const UAbleAbilityContext* Context, float deltaTime) const;
	
	virtual void OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context, const EAbleAbilityTaskResult result) const override;
	UFUNCTION(BlueprintNativeEvent)
	void OnTaskEndBP_Override(const UAbleAbilityContext* Context, const EAbleAbilityTaskResult result) const;

	virtual void DefaultPlaySound(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const;

	virtual bool IsDone(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override;

	virtual float GetEndTime() const override
	{
		return UAbleAbilityTask::GetEndTime();
	}

	virtual EAbleAbilityTaskRealm GetTaskRealm() const override { return GetTaskRealmBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "GetTaskRealm"))
	EAbleAbilityTaskRealm GetTaskRealmBP() const;
	
	virtual UAbleAbilityTaskScratchPad* CreateScratchPad(const TWeakObjectPtr<UAbleAbilityContext>& Context) const override;
 
	virtual TStatId GetStatId() const override;

	virtual void BindDynamicDelegates(UAbleAbility* Ability) override;

	virtual bool IsSingleFrame() const override { return IsSingleFrameBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "IsSingleFrame"))
	bool IsSingleFrameBP() const;

	virtual bool NeedsTick() const override {return !IsSingleFrame(); }

	virtual bool IsAsyncFriendly() const override {return false; }
	
#if WITH_EDITOR
	virtual FText GetTaskName() const override { return LOCTEXT("SPPlayParticleEffectTask", "Play Particle Effect With Sound"); }

	virtual EVisibility ShowEndTime() const { return EVisibility::Visible; }
#endif

protected:
	UPROPERTY(EditAnywhere, Category = "Related Sound", meta = (DisPlayName = "Target"))
	TEnumAsByte<EAbleAbilityTargetType> m_Target;
	
	UPROPERTY(EditAnywhere, Category = "Related Sound", meta = (DisPlayName = "Bank Name"))
	FString m_BankName;

	UPROPERTY(EditAnywhere, Category = "Related Sound", meta = (DisPlayName = "Play Event"))
	FString m_PlayEvent;

	UPROPERTY(EditAnywhere, Category = "Related Sound", meta = (DisPlayName = "Stop Event"))
	FString m_StopEvent;

	UPROPERTY(EditAnywhere, Category = "Related Sound", meta = (DisPlayName = "UnLoad CD"))
	float m_UnLoadCD;
};

#undef LOCTEXT_NAMESPACE
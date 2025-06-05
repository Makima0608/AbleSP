// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tasks/IAbleAbilityTask.h"
#include "Game/SPGame/Skill/Task/ShootProjectileTask.h"
#include "SPCreateBulletLineTask.generated.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

/* Scratchpad for this task. */
UCLASS(Transient)
class USPCreateBulletLineTaskScratchPad : public UAbleAbilityTaskScratchPad
{
	GENERATED_BODY()

public:
	USPCreateBulletLineTaskScratchPad();
	virtual ~USPCreateBulletLineTaskScratchPad() override;

	UPROPERTY()
	TArray<ASPProjectileBase*> SpawnedActors;

	UPROPERTY()
	TArray<FShootProjectileTaskSpawnInfo> SpawnInfos;

	UPROPERTY()
	int OutActorsNum;
};

USTRUCT()
struct FBulletLineLocation
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Projectile", meta = (DisplayName = "挂载物槽位"))
	FName Socket;

	UPROPERTY(EditAnywhere, Category = "Projectile", meta = (DisplayName = "挂载物枪口槽位"))
	FName MuzzleSocket = TEXT("Muzzle_point");
};

/**
 * 
 */
UCLASS()
class FEATURE_SP_API USPCreateBulletLineTask : public UAbleAbilityTask
{
	GENERATED_BODY()

public:
	USPCreateBulletLineTask(const FObjectInitializer& ObjectInitializer);
	virtual ~USPCreateBulletLineTask() override;

	/* Start our Task. */
	virtual void OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override;

	virtual void OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context, float DeltaTime) const override;

	/* End our Task. */
	virtual void OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
						   const EAbleAbilityTaskResult result) const override;

	virtual bool IsSingleFrame() const override { return BulletCDTime <= 0; }
	virtual UAbleAbilityTaskScratchPad* CreateScratchPad(const TWeakObjectPtr<UAbleAbilityContext>& Context) const override;
	virtual TStatId GetStatId() const override;

	virtual void SetSpawnParams(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
									FActorSpawnParameters& SpawnParams) const;

	virtual void AddOutActors(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
							  TArray<TWeakObjectPtr<AActor>>& OutActors) const;

	virtual void DestroyAtEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
							  USPCreateBulletLineTaskScratchPad*& ScratchPad) const;

#if WITH_EDITOR
	virtual FText GetTaskCategory() const override { return LOCTEXT("USPCreateBulletLineTask", "Weapon"); }
	virtual FText GetTaskName() const override { return LOCTEXT("USPCreateBulletLineTask", "CreateBulletLine"); }
	virtual bool CanEditTaskRealm() const override { return true; }
	virtual FLinearColor GetTaskColor() const override { return FLinearColor(255.0f, 0.0f, 220.0f / 255.0f); }
#endif

protected:
	FVector GetProjectileDirection(AActor* Owner) const;
	void CreateBulletLine(const TWeakObjectPtr<const UAbleAbilityContext>& Context, FVector StartPos, FVector ShootDirection) const;
	void SpawnProjectiles(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const;
	void ApplyShotSpread(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const;
	ASPGameCharacterBase* GetPlayer(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const;

	void InitSpawnTargetLocationForTargetActor(const TWeakObjectPtr<const UAbleAbilityContext>& Context, int32 TargetIndex, TArray<FTransform>& OutTransform) const;
	bool IsKeepFiring() const;

	void OnSingleShoot(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const;

protected:
	UPROPERTY(EditAnywhere, Category = "Spawn", meta = (DisplayName = "SocketLocationList"))
	TArray<FBulletLineLocation> SpawnSocketLocationList;

	UPROPERTY(EditAnywhere, Category = "Spawn", meta = (DisplayName = "Owner"))
	TEnumAsByte<EAbleAbilityTargetType> OwnerTargetType;

	UPROPERTY(EditAnywhere, Category = "Spawn", meta = (DisplayName = "Amount to Spawn", UIMin=1, ClampMin=1))
	int AmountToSpawn = 1;

	UPROPERTY(EditAnywhere, Category = "Projectile|Override", meta = (DisplayName = "是否开启散射"))
	bool bEnableShotSpread = false;

	UPROPERTY(EditAnywhere, Category = "Spawn", meta = (DisplayName = "子弹间隔（0：单帧）"))
	float BulletCDTime = 0.f;

private:
	mutable float LastShootTime = 0.f;
};

#undef LOCTEXT_NAMESPACE
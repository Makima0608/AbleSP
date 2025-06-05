#pragma once

#include "UnLuaInterface.h"
#include "CoreMinimal.h"
#include "ableAbility.h"
#include "MoeGameLog.h"
#include "Game/SPGame/Utils/SPGameLibrary.h"
#include "Tasks/IAbleAbilityTask.h"
#include "ShootProjectileTask.generated.h"

/**
 * 
 */
#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

class ASPProjectileBase;
class UAbleAbilityContext;

DECLARE_DYNAMIC_DELEGATE_RetVal_TwoParams(TSubclassOf<ASPProjectileBase>, FGetSPProjectileClass,
                                          const UAbleAbilityContext*, Context, TSubclassOf<ASPProjectileBase>,
                                          StaticValue);

DECLARE_DYNAMIC_DELEGATE_RetVal_TwoParams(ESPShootRotationType, FGetSPShootRotationType, const UAbleAbilityContext*,
                                          Context, ESPShootRotationType, StaticValue);

DECLARE_DYNAMIC_DELEGATE_RetVal_TwoParams(FVector2D, FGetSPCrossHairAngleConstraint, const UAbleAbilityContext*,
										  Context, FVector2D, StaticValue);

DECLARE_DYNAMIC_DELEGATE_RetVal_TwoParams(FVector2D, FGetSPOnlyAdjustZ, const UAbleAbilityContext*,
										  Context, bool, StaticValue);

DECLARE_DYNAMIC_DELEGATE_RetVal_TwoParams(ESPShootType, FGetSPShootType, const UAbleAbilityContext*, Context,
                                          ESPShootType, StaticValue);

DECLARE_DYNAMIC_DELEGATE_RetVal_TwoParams(FVector, FGetSPVector, const UAbleAbilityContext*, Context, FVector,
                                          StaticValue);

DECLARE_DYNAMIC_DELEGATE_RetVal_TwoParams(FProjectileBaseInfo, FGetSPProjectileInfo, const UAbleAbilityContext*,
                                          Context, FProjectileBaseInfo, StaticValue);

UENUM(BlueprintType)
enum class ECaptureFunctionType : uint8
{
	Capture = 0 UMETA(DisplayName = "抓捕"),
	Unleash UMETA(DisplayName = "召唤"),
	Leash UMETA(DisplayName = "收回"),
};

UENUM(BlueprintType)
enum class EProjectileTargetTeamModeType : uint8
{
	None = 0 UMETA(DisplayName = "无限制"),
	SameTeam UMETA(DisplayName = "相同队伍"),
	DifferentTeam UMETA(DisplayName = "不同队伍"),
};

UENUM(BlueprintType)
enum class EAimTargetLocationType : uint8
{
	AimTowardSTP = 0 UMETA(DisplayName = "朝向虚拟点发射"),
	AimTowardTargetLocation  UMETA(DisplayName = "朝向技能目标点发射"),
};

USTRUCT()
struct FShootProjectileTaskSpawnInfo{
	GENERATED_BODY()

	FShootProjectileTaskSpawnInfo(){}
	
	FShootProjectileTaskSpawnInfo(TSubclassOf<AActor> InActorClass, FTransform InSpawnTransform, FActorSpawnParameters InSpawnParams)
	{
		ActorClass = InActorClass;
		SpawnParams = InSpawnParams;
		SpawnTransform = InSpawnTransform;
	}

	FShootProjectileTaskSpawnInfo(TSubclassOf<AActor> InActorClass, TArray<FTransform>& InSpawnTransformList, FActorSpawnParameters InSpawnParams)
	{
		ActorClass = InActorClass;
		SpawnParams = InSpawnParams;
		SpawnTransformList = InSpawnTransformList;
	}
	
	TSubclassOf<AActor> ActorClass;
	FTransform SpawnTransform;
	TArray<FTransform> SpawnTransformList;
	FActorSpawnParameters SpawnParams;

	FTransform GetSpawnTransformByIndex(const int32 Index)
	{
		if (Index >= 0 && Index < SpawnTransformList.Num())
		{
			return SpawnTransformList[Index];
		}
		else
		{
			MOE_LOG_ERROR(TEXT("勾选了bUseSeparateLocation独立位置模式， 但m_AmountToSpawn与SpawnLocationList不匹配！！！"));
		}
		return SpawnTransform;
	}
};

/* Scratchpad for this task. */
UCLASS(Transient)
class UShootProjectileTaskScratchPad : public UAbleAbilityTaskScratchPad
{
	GENERATED_BODY()

public:
	UShootProjectileTaskScratchPad();
	virtual ~UShootProjectileTaskScratchPad();

	/* The Actor(s) we've spawned. */
	UPROPERTY()
	TArray<ASPProjectileBase*> SpawnedActors;

	UPROPERTY()
	FTimerHandle SpawnTimer;
	
	UPROPERTY()
	TArray<FShootProjectileTaskSpawnInfo> SpawnInfos; //每个outactor对应一个SpawnInfo

	UPROPERTY()
	int OutActorsNum;
	
	UPROPERTY()
	int CurrentShootTimes;			// 当前发射次数

	UPROPERTY()
	float CurTimeStamp;   // 累计的发射间隔时间
};

UCLASS()
class FEATURE_SP_API UShootProjectileTask : public UAbleAbilityTask, public IUnLuaInterface
{
	GENERATED_BODY()

protected:
	virtual FString GetModuleName_Implementation() const override;

public:
	UShootProjectileTask(const FObjectInitializer& ObjectInitializer);
	virtual ~UShootProjectileTask();

	/* Start our Task. */
	virtual void OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskStart"))
	void OnTaskStartBP(const UAbleAbilityContext* Context) const;

	UFUNCTION(BlueprintImplementableEvent)
	void OnCalcMaxDamageCount(const UAbleAbilityContext* Context) const;
	
	/* Tick our Task. */
	virtual void OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context, float deltaTime) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskTick"))
	void OnTaskTickBP(const UAbleAbilityContext* Context, float deltaTime) const;

	/* End our Task. */
	virtual void OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	                       const EAbleAbilityTaskResult result) const override;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnTaskEnd"))
	void OnTaskEndBP(const UAbleAbilityContext* Context,
					 const EAbleAbilityTaskResult result) const;

	/* Returns true if our Task only lasts a single frame. */
	virtual bool IsSingleFrame() const override { return IsSingleFrameBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "IsSingleFrame"))
	bool IsSingleFrameBP() const;

	/* Returns the realm our Task belongs to. */
	virtual EAbleAbilityTaskRealm GetTaskRealm() const override { return GetTaskRealmBP(); }

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "GetTaskRealm"))
	EAbleAbilityTaskRealm GetTaskRealmBP() const;

	/* Creates the Scratchpad for this Task. */
	virtual UAbleAbilityTaskScratchPad* CreateScratchPad(const TWeakObjectPtr<UAbleAbilityContext>& Context) const;

	/* Returns the Profiler Stat ID for this Task. */
	virtual TStatId GetStatId() const override;

	virtual void BindDynamicDelegates(UAbleAbility* Ability) override;

	virtual void SetSpawnParams(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	                            FActorSpawnParameters& SpawnParams) const;

	virtual void JudgeShootType(ASPProjectileBase*& SpawnedActor) const;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "LoopSpawnProjectilesByTimeSpan"))
	void LoopSpawnProjectilesByTimeSpanBP(const UAbleAbilityContext* Context) const;

	virtual void AddOutActors(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	                          TArray<TWeakObjectPtr<AActor>>& OutActors) const;

	virtual void DestroyAtEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	                          UShootProjectileTaskScratchPad*& ScratchPad) const;

	/* Returns the End time of our Task. */
    virtual float GetEndTime() const override;

#if WITH_EDITOR
	/* Returns the category of our Task. */
	virtual FText GetTaskCategory() const override { return LOCTEXT("SPSkillSpawnActorTaskCategory", "Spawn"); }

	/* Returns the name of our Task. */
	virtual FText GetTaskName() const override { return LOCTEXT("SPSkillShootProjectileTask", "Shoot Projectile"); }

	/* Returns the dynamic, descriptive name of our Task. */
	virtual FText GetDescriptiveTaskName() const override;

	/* Returns the description of our Task. */
	virtual FText GetTaskDescription() const override
	{
		return LOCTEXT("SPSkillShootProjectileTaskDesc", "Shoot Projectile.");
	}

	virtual EVisibility ShowEndTime() const override { return m_DestroyAtEnd || m_bManuallySpecifyEndTime ? EVisibility::Visible : EVisibility::Hidden; }
	

	/* Returns a Rich Text version of the Task summary, for use within the Editor. */
	virtual FText GetRichTextTaskSummary() const;

	/* Returns the color of our Task. */
	virtual FLinearColor GetTaskColor() const override { return FLinearColor(0.0f, 0.0f, 220.0f / 255.0f); }

	/* Returns the estimated runtime cost of our Task. */
	virtual float GetEstimatedTaskCost() const override
	{
		return UAbleAbilityTask::GetEstimatedTaskCost() + ABLETASK_EST_SPAWN_ACTOR;
	}

	/* Data Validation Tests. */
	EDataValidationResult IsTaskDataValid(const UAbleAbility* AbilityContext, const FText& AssetName,
	                                      TArray<FText>& ValidationErrors) override;

	/* Returns true if the user is allowed to edit the realm for this Task. */
	virtual bool CanEditTaskRealm() const override { return true; }
#endif

protected:
	/* The class of the actor we want to dynamically spawn. */
	UPROPERTY(EditAnywhere, Category = "Spawn", meta = (DisplayName = "Actor Class", AbleBindableProperty))
	TSoftClassPtr<AActor> m_ActorClass;

	UPROPERTY()
	FGetAbleActor m_ActorClassDelegate;
	
	UPROPERTY(EditAnywhere, Category = "Spawn", meta = (DisplayName = "Is For Capture"))
	bool m_bIsForCapture;

	UPROPERTY(EditAnywhere, Category = "Spawn", meta = (DisplayName = "Capture Type", EditCondition = "m_bIsForCapture==true"))
	ECaptureFunctionType m_CaptureFunctionType;

	UPROPERTY(EditAnywhere, Category = "Spawn", meta = (DisplayName = "Cost Item"))
	bool m_bCostItem;

	UPROPERTY(EditAnywhere, Category = "Spawn", meta = (DisplayName = "Cost ItemID",EditCondition = "m_bCostItem==true", ShortTooltip = "默认配置-1为无效"))
	int64 m_CostItemId;

	UPROPERTY(EditAnywhere, Category = "Spawn",
		meta = (DisplayName = "Amount to Spawn", AbleBindableProperty, UIMin=1, ClampMin=1))
	int m_AmountToSpawn;

	UPROPERTY()
	FGetAbleInt m_AmountToSpawnDelegate;

	UPROPERTY(EditAnywhere, Category = "Spawn", meta = (DisplayName = "Use Weapon Socket"))
	bool m_bUseWeaponLocation = false;

	// todo - rectify property name for better readibility
	UPROPERTY(EditAnywhere, Category = "Spawn", meta = (DisplayName = "Use Weapon Location",EditCondition = "m_bUseWeaponLocation"))
	bool m_bUseWeaponSocketLocation = true;

	UPROPERTY(EditAnywhere, Category = "Spawn", meta = (DisplayName = "Use Weapon Rotation",EditCondition = "m_bUseWeaponLocation"))
	bool m_bUseWeaponSocketRotation = true;

	UPROPERTY(EditAnywhere, Category = "Spawn", meta = (DisplayName = "Use Weapon Socket Location"))
	FName m_WeaponSocketLocation;

	UPROPERTY(EditAnywhere, Category = "Projectile", meta = (DisplayName = "Damage Index", AbleBindableProperty))
	int32 m_DamageIndex = -1;

	UPROPERTY()
	FGetAbleInt m_DamageIndexDelegate;

	UPROPERTY(EditAnywhere, Category = "Projectile", meta = (DisplayName = "Damage Id(保底用，会优先通过DamageIndex找伤害ID，无效时读取该值)", AbleBindableProperty))
	int32 m_DamageId = -1;

	UPROPERTY()
	FGetAbleInt m_DamageIdDelegate;

	UPROPERTY(EditAnywhere, Category = "Projectile|Override",
		meta = (DisplayName = "Override Projectile", AbleBindableProperty))
	bool m_OverrideProjectile = false;

	UPROPERTY()
	FGetAbleBool m_OverrideProjectileDelegate;

	UPROPERTY(EditAnywhere, Category = "Projectile|Override",
		meta = (DisplayName = "ShootRotation Type", AbleBindableProperty))
	ESPShootRotationType m_ShootRotationType = ESPShootRotationType::UseSelfRotation;

	UPROPERTY()
	FGetSPShootType m_ShootRotationTypeDelegate;

	UPROPERTY(EditAnywhere, Category = "Projectile|Override",
		meta = (DisplayName = "俯角发射限制(x,y)", AbleBindableProperty, EditCondition = "m_ShootRotationType == ESPShootRotationType::UseCrosshairDirection || m_ShootRotationType == ESPShootRotationType::UsePlayerAimingPoint"))
	FVector2D m_CrossHairAngleConstraint = FVector2D(0.);

	UPROPERTY()
	FGetSPCrossHairAngleConstraint m_CrossHairAngleConstraintDelegate;
	
	UPROPERTY(EditAnywhere, Category = "Projectile|Override",
		meta = (DisplayName = "是否只调整发射方向的Z值", AbleBindableProperty, EditCondition = "m_ShootRotationType == ESPShootRotationType::UseCrosshairDirection || m_ShootRotationType == ESPShootRotationType::UsePlayerAimingPoint"))
	bool m_bOnlyAdjustZ = false;

	UPROPERTY()
	FGetSPOnlyAdjustZ m_bOnlyAdjustZDelegate;

	UPROPERTY(EditAnywhere, Category = "Projectile|Override",
		meta = (DisplayName = "ShootRotation Type Backup",EditCondition = "m_ShootRotationType == ESPShootRotationType::UseTargetPosition", AbleBindableProperty))
	ESPShootRotationType m_ShootRotationTypeBackUp = ESPShootRotationType::UseSelfRotation;

	UPROPERTY()
	FGetSPShootType m_ShootRotationTypeBackUpDelegate;
	

	UPROPERTY(EditAnywhere, Category = "Projectile", meta = (DisplayName = "Shoot Type", AbleBindableProperty))
	ESPShootType m_ShootType = ESPShootType::Auto;

	UPROPERTY()
	FGetSPShootType m_ShootTypeDelegate;

	UPROPERTY(EditAnywhere, Category = "Projectile",
		meta = (DisplayName = "Shoot Delay", AbleBindableProperty, EditCondition = "m_ShootType==ESPShootType::Timed"))
	float m_ShootDelay = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Projectile",
		meta = (DisplayName = "Spawn Delay", AbleBindableProperty))
	float m_SpawnDelay = 0.0f;

	UPROPERTY()
	FGetAbleFloat m_ShootDelayDelegate;

	UPROPERTY()
	FGetAbleFloat m_SpawnDelayDelegate;

	UPROPERTY(EditAnywhere, Category = "Projectile",
		meta = (DisplayName = "Aim Target Location", EditCondition =
			"m_ShootRotationType==ESPShootRotationType::UseShootDirection", EditConditionHides,
			ToolTip = "只有在'Shoot Rotation Type'的选项等于'使用发射方向'时，才能更动, Enabled时，技能会向子弹生成位置一定偏移量的目标点方向发射投射物。"))
		bool m_AimTowardTargetLocation = false;

	UPROPERTY(EditAnywhere, Category = "Projectile",
		meta = (DisplayName = "Fire Direction", AbleBindableProperty, EditCondition =
			"m_AimTowardTargetLocation==false",
			ToolTip = "只有在'Shoot Rotation Type'的选项等于'使用发射方向'时，才必然读取，见'Absolute Fire Direction'注解。"))
		FVector m_ShootDirection = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "Projectile",
		meta = (DisplayName = "Absolute Fire Direction", EditCondition = "m_AimTowardTargetLocation==false", ToolTip = "Enabled时，技能会向'Fire Direction'提供的世界坐标的方向发射投射物。"))
		bool m_AbsoluteDirection = true;

	UPROPERTY(EditAnywhere, Category = "Projectile",
		meta = (DisplayName = "Aim Target Location Type", EditCondition =
			"m_ShootRotationType==ESPShootRotationType::UseShootDirection&&m_AimTowardTargetLocation==true", EditConditionHides))
		EAimTargetLocationType m_AimTargetLocationType;

	UPROPERTY(EditAnywhere, Category = "Projectile",
		meta = (DisplayName = "Aim Target Location", EditCondition =
			"m_AimTowardTargetLocation==true&&m_AimTargetLocationType==EAimTargetLocationType::AimTowardTargetLocation",
			ToolTip = "只有在'Aim Target Location'的选项等于true时，才能更动,投射物朝向目标点的位置", EditConditionHides))
		FAbleAbilityTargetTypeLocation m_AimTargetLocation;

	UPROPERTY(EditAnywhere, Category = "Projectile",
		meta = (DisplayName = "DungeonSTPUID", EditCondition =
			"m_AimTowardTargetLocation==true&&m_AimTargetLocationType==EAimTargetLocationType::AimTowardSTP",
			ToolTip = "同DungeonSTPID,读取更快,推荐配置数字ID,只有在'Aim Target Location'的选项等于true时，才能更动,投射物朝向大世界虚拟点的位置", EditConditionHides))
		int32 STPUID;

	UPROPERTY(EditAnywhere, Category = "Projectile",
		meta = (DisplayName = "DungeonSTPID", EditCondition =
			"m_AimTowardTargetLocation==true&&m_AimTargetLocationType==EAimTargetLocationType::AimTowardSTP",
			ToolTip = "只有在'Aim Target Location'的选项等于true时，才能更动,投射物朝向大世界虚拟点的位置", EditConditionHides))
		FString STPID;

	UPROPERTY()
	FGetSPVector m_ShootDirectionDelegate;

	UPROPERTY(EditAnywhere, Category = "Projectile|Override",
		meta = (DisplayName = "Projectile Info", AbleBindableProperty))
	FProjectileBaseInfo m_ProjectileBaseInfo = FProjectileBaseInfo();

	UPROPERTY()
	FGetSPProjectileInfo m_ProjectileBaseInfoDelegate;

	UPROPERTY(EditAnywhere, Category = "Projectile",
		meta = (DisplayName = "Aiming Target"))
	TEnumAsByte<EAbleAbilityTargetType> m_AimingTarget;

	UPROPERTY(EditAnywhere, Category = "Projectile",
		meta = (DisplayName = "Aiming Target Socket", EditCondition =
			"m_ShootRotationType==ESPShootRotationType::UseTargetPosition"))
	FName m_AimingTargetSocket;

	UPROPERTY(EditAnywhere, Category = "Projectile",
		meta = (DisplayName = "Aim Sole Target", 
			ToolTip = "Enable时，若AimingTarget为ATT_TargetActor,即使有多个target，子弹也会只向最后一个target发射", 
			EditCondition = "m_AimingTarget == EAbleAbilityTargetType::ATT_TargetActor"))
		bool m_SoleTarget;

	UPROPERTY(EditAnywhere, Category = "Projectile",
		meta = (DisplayName = "AlwaysSpawnWhenTargetEmpty",
			ToolTip = "Enable时，如果没有TatgetActor也会发射子弹",
			EditCondition = "m_ShootRotationType==ESPShootRotationType::UseTargetPosition && m_AimingTarget == EAbleAbilityTargetType::ATT_TargetActor"))
		bool m_SpawnWhenTargetEmpty;

	UPROPERTY(EditAnywhere, Category = "Projectile", meta = (DisplayName = "InnoreLocationSourceActor",
		ToolTip = "Enable时，忽略生成位置的Actor"))
		bool m_InnoreLocationSourceActor;

	UPROPERTY(EditAnywhere, Category = "Projectile", meta = (DisplayName = "Target Use Master Weapon"))
	bool m_bUseWeaponTarget = false;

	UPROPERTY(EditAnywhere, Category = "Projectile", meta = (DisplayName = "Target Use Master Weapon Socket Name"))
	FName m_WeaponTargetSocket;

	UPROPERTY(EditAnywhere, Category = "Projectile", meta = (DisplayName = "TargetTeamMode",
		ToolTip = "是否使用TeamId过滤Target"))
		EProjectileTargetTeamModeType m_TargetTeamMode = EProjectileTargetTeamModeType::None;

	UPROPERTY(EditAnywhere, Category = "Spawn", meta = (DisplayName = "Spawn Collision"))
	ESpawnActorCollisionHandlingMethod m_SpawnCollision;

	UPROPERTY(EditAnywhere, Category = "Spawn", meta = (DisplayName = "使用独立的Location", EditCondition = "m_AmountToSpawn > 1", EditConditionHides))
	bool bUseSeparateLocation = false;

	/* The socket within our mesh component to attach the actor to, or use as an initial transform when we spawn the Actor */
	UPROPERTY(EditAnywhere, Category = "Spawn", meta = (DisplayName = "Location", AbleBindableProperty, EditCondition = "m_AmountToSpawn == 1 || !bUseSeparateLocation", EditConditionHides))
	FAbleAbilityTargetTypeLocation m_SpawnLocation;

	UPROPERTY(EditAnywhere, Category = "Spawn", meta = (DisplayName = "LocationList", EditCondition = "m_AmountToSpawn > 1 && bUseSeparateLocation", EditConditionHides))
	TArray<FAbleAbilityTargetTypeLocation> SpawnLocationList;

	UPROPERTY()
	FGetAbleTargetLocation m_SpawnLocationDelegate;

	/* If true, Set the owner of the new actor. */
	UPROPERTY(BlueprintReadOnly, Category = "Spawn", meta = (DisplayName = "Set Owner(不生效)"))
	bool m_SetOwner;

	/* The parent of the Actor. */
	UPROPERTY(EditAnywhere, Category = "Spawn", meta = (DisplayName = "Owner", EditCondition = "m_SetOwner"))
	TEnumAsByte<EAbleAbilityTargetType> m_OwnerTargetType;

	/* If true, marks the Actor as transient, so it won't be saved between game sessions. */
	UPROPERTY(EditAnywhere, Category = "Spawn", meta = (DisplayName = "Transient"))
	bool m_MarkAsTransient;

	/* Whether or not we destroy the actor at the end of the task. */
	UPROPERTY(EditAnywhere, Category = "Spawn", meta = (DisplayName = "Destroy On End"))
	bool m_DestroyAtEnd;

	/* If true, we'll call the OnSpawnedActorEvent in the Ability Blueprint. */
	UPROPERTY(EditAnywhere, Category = "Spawn|Event", meta = (DisplayName = "Fire Event"))
	bool m_FireEvent;

	/* A String identifier you can use to identify this specific task in the Ability blueprint. */
	UPROPERTY(EditAnywhere, Category = "Spawn|Event", meta = (DisplayName = "Name", EditCondition = m_FireEvent))
	FName m_Name;

	/* What realm, server or client, to execute this task. If your game isn't networked - this field is ignored. */
	UPROPERTY(EditAnywhere, Category = "Realm", meta = (DisplayName = "Realm", EditCondition = "m_ShootRotationType != ESPShootRotationType::UseCrosshairDirection", EditConditionHides))
	TEnumAsByte<EAbleAbilityTaskRealm> m_TaskRealm;

	UPROPERTY(EditAnywhere, Category = "Projectile|Override", meta = (DisplayName = "Custom Range", EditCondition = "m_ShootRotationType == ESPShootRotationType::UseCrosshairDirection", EditConditionHides))
	bool bUseCustomRange = false;

	UPROPERTY(EditAnywhere, Category = "Projectile|Override", meta = (EditCondition = "m_ShootRotationType == ESPShootRotationType::UseCrosshairDirection && bCustomRange", EditConditionHides))
	float WeaponRange = 2000.f;
	
	UPROPERTY(EditAnywhere, Category = "Projectile|Override", meta = (DisplayName = "是否开启散射", EditCondition = "m_ShootRotationType == ESPShootRotationType::UseCrosshairDirection", EditConditionHides))
	bool bEnableShotSpread = false;

	UPROPERTY(EditAnywhere, Category = "Spawn", meta = (DisplayName = "是否用于骑乘"))
	bool bIsRideMode = false;

	UPROPERTY(EditAnywhere, Category = "Spawn", meta = (DisplayName = "Loop Shoot", ToolTip = "是否开启循环发射"))
	bool m_bLoopShoot = false;

	UPROPERTY(EditAnywhere, Category = "Timing", meta = (DisplayName = "是否手动设置Task结束时刻", ToolTip = "是否手动设置Task结束时刻"))
	bool m_bManuallySpecifyEndTime = false;

	UPROPERTY(EditAnywhere, Category = "Spawn", meta = (DisplayName = "Interval Time", ToolTip = "循环发射间隔时间", EditCondition = m_bLoopShoot, ClampMin = 0.0))
	float m_IntervalTime;

	UPROPERTY(EditAnywhere, Category = "Spawn", meta = (DisplayName = "Total Shoot Times", ToolTip = "总发射次数", EditCondition = m_bLoopShoot, ClampMin = 1))
	int32 m_TotalShootTimes;

	UPROPERTY(EditAnywhere, Category = "Projectile", meta = (DisplayName = "Is RealDamage", TooITip = "是否造成真实伤害"))
	bool m_bIsRealDamage = false;

	UPROPERTY(EditAnywhere, Category = "Projectile", meta = (DisplayName = "RealDamage Ratio", ToolTip = "真实伤害比率", EditCondition = m_bIsRealDamage, ClampMin = 0.0, ClampMax = 100.0))
	float m_RealDamageRatio = 0.;

	UPROPERTY(EditAnywhere, Category = "Spawn", meta = (DisplayName = "Specify Target Index", ToolTip = "是否指定投射物目标"))
	bool m_bSpecifyTargetIndex = false;

	UPROPERTY(EditAnywhere, Category = "Spawn", meta = (DisplayName = "Clamp Target Index", ToolTip = "如果指定的目标不存在，是否将其吸附到最近的有效索引,不吸附则跳过生成", EditCondition = m_bSpecifyTargetIndex))
	bool m_bClampTargetIndex = false;

	UPROPERTY(EditAnywhere, Category = "Spawn", meta = (DisplayName = "Target Index", ToolTip = "目标索引", EditCondition = m_bSpecifyTargetIndex))
	int32 m_TargetIndex = 0;

protected:
	FVector GetProjectileDirection(const TWeakObjectPtr<const UAbleAbilityContext>& Context, FVector SpawnTransform) const;
	FVector GetCrossHairDirection(AActor* Owner) const;

	void GetAimingTarget(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
	                     TWeakObjectPtr<AActor>& OutActor, int SpawnIndex) const;
	
	void SpawnProjectiles(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const;

	void SpawnProjectilesForTarget(const TWeakObjectPtr<const UAbleAbilityContext>& Context, const FSPSkillInfo& SkillInfo, int32 TargetIndex, int32 NumToSpawn = 1) const;

	TSubclassOf<AActor> CaptureOverride(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const;

	void CaptureOverrideShootDirection(const TWeakObjectPtr<const UAbleAbilityContext>& Context, ASPProjectileBase* Projectile) const;
	
	int64 GetCaptureCostItemId(const TWeakObjectPtr<const UAbleAbilityContext>& Context, int64 CostItemId) const;

	void AddIgnoreActors(const TWeakObjectPtr<const UAbleAbilityContext>& Context, ASPProjectileBase*& SpawnedActor) const;

	bool CanApplyShotSpread() const;

	void ApplyShotSpread(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const;

	TEnumAsByte<EAbleAbilityTaskRealm> Internal_GetTaskRealm() const;

	bool IsSeparateLocation() const;

	void InitSpawnTargetLocationForTargetActor(const UAbleAbilityContext& Context, int32 TargetIndex, TArray<FTransform>& OutTransform) const;

	ASPGameCharacterBase* GetPlayer(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Projectile")
	FVector GetProjectileAimLocation_lua(const AActor* Owner) const;

	FTransform GetSpawnTransform(FShootProjectileTaskSpawnInfo& SpawnInfo, int32 SpawnIndex) const;
	void RefreshByWeaponLocation(const TWeakObjectPtr<const UAbleAbilityContext>& Context, FTransform& OutTransform) const;
	bool ApplyWeaponTarget(const TWeakObjectPtr<const UAbleAbilityContext>& Context, ASPProjectileBase* SpawnedActor) const;
	bool IsSpawnLocally(EProjectileNetPolicy NetPolicy) const;
	bool IsCostItem() const;
};

#undef LOCTEXT_NAMESPACE

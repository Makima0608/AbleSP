#include "Game/SPGame/Skill/Task/SPJumpAttachRootMotionTask.h"

#include "HAL/IConsoleManager.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Curves/CurveVector.h"
#include "Curves/CurveFloat.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"

#include "ableSubSystem.h"

#if !(UE_BUILD_SHIPPING)
#include "ableAbilityUtilities.h"
#endif

#include "Game/SPGame/Utils/SPUnifiedNetworkLibrary.h"
#include "Game/SPGame/Common/JumpAttach/SPRootMotionSource_Jump.h"



DECLARE_STATS_GROUP(TEXT("SPRootMotionAbility"), STATGROUP_SPRootMotionAbility, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("SPJumpAttachRootMotionTask"), STAT_SPJumpAttachRootMotionTask, STATGROUP_SPRootMotionAbility);

#if !UE_BUILD_SHIPPING
static TAutoConsoleVariable<int32> CVarDebugJumpAttachLerping(
	TEXT("sp.DebugJumpAttachLerping"),
	0,
	TEXT("Whether to draw jump attach lerping information.\n")
	TEXT("0: Disable, 1: Enable"),
	ECVF_Cheat);

static TAutoConsoleVariable<int32> CVarOverrideJumpAttach(
	TEXT("sp.OverrideJumpAttach"),
	0,
	TEXT("Whether to override jump attach parameters.\n")
	TEXT("0: Disable, 1: Enable"),
	ECVF_Cheat);

static TAutoConsoleVariable<float> CVarJumpAttachHeightFactor(
	TEXT("sp.JumpAttachHeightFactor"),
	0.25f,
	TEXT("JumpAttachHeightFactor.\n")
	TEXT(""),
	ECVF_Cheat);

static TAutoConsoleVariable<float> CVarJumpAttachHeightOffset(
	TEXT("sp.JumpAttachHeightOffset"),
	100.f,
	TEXT("JumpAttachHeightOffset.\n")
	TEXT(""),
	ECVF_Cheat);

static TAutoConsoleVariable<float> CVarJumpAttachMinJumpHeight(
	TEXT("sp.JumpAttachMinJumpHeight"),
	100.f,
	TEXT("JumpAttachMinJumpHeight.\n")
	TEXT(""),
	ECVF_Cheat);

static TAutoConsoleVariable<float> CVarJumpAttachRadiusSquared(
	TEXT("sp.JumpAttachRadiusSquared"),
	2500.f,
	TEXT("JumpAttachRadiusSquared.\n")
	TEXT(""),
	ECVF_Cheat);

static TAutoConsoleVariable<float> CVarJumpAttachCollisionExtentFactorX(
	TEXT("sp.JumpAttachCollisionExtentFactorX"),
	0.f,
	TEXT("JumpAttachCollisionExtentX.\n")
	TEXT(""),
	ECVF_Cheat);

static TAutoConsoleVariable<float> CVarJumpAttachCollisionExtentFactorY(
	TEXT("sp.JumpAttachCollisionExtentFactorY"),
	0.f,
	TEXT("JumpAttachCollisionExtentY.\n")
	TEXT(""),
	ECVF_Cheat);

static TAutoConsoleVariable<float> CVarJumpAttachCollisionExtentFactorZ(
	TEXT("sp.JumpAttachCollisionExtentFactorZ"),
	1.f,
	TEXT("JumpAttachCollisionExtentFactorZ.\n")
	TEXT(""),
	ECVF_Cheat);
#endif// !UE_BUILD_SHIPPING

#define LOCTEXT_NAMESPACE "SPAbleAbilityTask"


USPJumpAttachRootMotionTaskScratchPad::USPJumpAttachRootMotionTaskScratchPad()
	: RootMotionSourceID(0u)
	, bAttachLerping(false)
	, bRotationLerping(false)
	, OldNetSmoothMode(ENetworkSmoothingMode::Exponential)
	, JumpPeakTime(0.f)
	, DefaultOwnerMeshLoc(EForceInit::ForceInit)
	, LerpEndOwnerMeshLoc(EForceInit::ForceInit)
	, DefaultOwnerMeshQuat(EForceInit::ForceInit)
	, LerpEndOwnerMeshQuat(EForceInit::ForceInit)
	, OwnerJumpStartRotationInTargetRootSpace(EForceInit::ForceInit)
{
}


void USPJumpAttachRootMotionTaskScratchPad::ResetPad_Implementation()
{
	RootMotionSourceID = 0u;
	bAttachLerping = false;
	bRotationLerping = false;
	OldNetSmoothMode = ENetworkSmoothingMode::Exponential;
	JumpPeakTime = 0.f;
	DefaultOwnerMeshLoc = FVector::ZeroVector;
	LerpEndOwnerMeshLoc = FVector::ZeroVector;
	DefaultOwnerMeshQuat = FQuat::Identity;
	LerpEndOwnerMeshQuat = FQuat::Identity;
	OwnerJumpStartRotationInTargetRootSpace = FQuat::Identity;
	TargetMeshCompPtr.Reset();
	OldMeshParentCompPtr.Reset();
	SocketName = NAME_None;
}


USPJumpAttachRootMotionTask::USPJumpAttachRootMotionTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, m_CollisionExtentAttachFactor(0, 0, 1.f)
	, m_TargetHeightFactor(0.25f)
	, m_TargetHeightOffset(100.f)
	, m_MinJumpHeight(100.f)
	, m_AttachRadiusSquared(2500.f)
	, m_StartRotationTime(0.f)
	, m_PathOffsetCurve(nullptr)
	, m_TimeMappingCurve(nullptr)
	, m_AttachLerpingCurve(nullptr)
{
}


FString USPJumpAttachRootMotionTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPJumpAttachRootMotionTask");
}

void USPJumpAttachRootMotionTask::OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const
{
	check(Context);

	USPJumpAttachRootMotionTaskScratchPad* ScratchPad = Cast<USPJumpAttachRootMotionTaskScratchPad>(GetScratchPad(const_cast<UAbleAbilityContext*>(Context)));

	if (!ScratchPad)
	{
		return;
	}

	ACharacter* OwnerPawn = Cast<ACharacter>(Context->GetOwner());

	if (!OwnerPawn)
	{
		return;
	}

	UPrimitiveComponent* OwnerMoveUpdateComp = Cast<UPrimitiveComponent>(OwnerPawn->GetRootComponent());
	USkeletalMeshComponent* OwnerMeshComp = OwnerPawn->GetMesh();
	UCharacterMovementComponent* MoveComp = OwnerPawn->GetCharacterMovement();

	if (!MoveComp || !OwnerMoveUpdateComp)
	{
		return;
	}
	// 获取目标
	UPrimitiveComponent* TargetMoveUpdateComp = nullptr;
	UPrimitiveComponent* TargetMeshComponent = Cast<UPrimitiveComponent>(Context->GetUObjectParameter(m_TargetCompParamName));
	const FString* SocketNameStrPtr = Context->GetStringParameters().Find(m_SocketNameParamName);
	FName SocketName = SocketNameStrPtr ? FName(**SocketNameStrPtr) : m_SocketName;
	ScratchPad->SocketName = SocketName;
	
	if (IsValid(TargetMeshComponent) && TargetMeshComponent->GetOwner())
	{
		TargetMoveUpdateComp = Cast<UPrimitiveComponent>(TargetMeshComponent->GetOwner()->GetRootComponent());
	}
	else
	{
		if (!Context->HasAnyTargets())
		{
			return;
		}

		const TArray<AActor*>& TargetActorList = Context->GetTargetActors();

		if (TargetActorList.Num() <= 0)
		{
			return;
		}

		AActor* TargetActor = TargetActorList[0];

		if (!IsValid(TargetActor))
		{
			return;
		}

		if (ACharacter* TargetCharacter = Cast<ACharacter>(TargetActor))
		{
			TargetMeshComponent = TargetCharacter->GetMesh();
		}
		else
		{
			TargetMeshComponent = TargetActor->FindComponentByClass<UPrimitiveComponent>();
		}

		TargetMoveUpdateComp = Cast<UPrimitiveComponent>(TargetActor->GetRootComponent());
	}

	if (!TargetMoveUpdateComp || !TargetMeshComponent)
	{
		return;
	}

#if !UE_BUILD_SHIPPING
	float TargetHeightFactorRef = m_TargetHeightFactor;
	float TargetHeightOffsetRef = m_TargetHeightOffset;
	float TargetMinJumpHeightRef= m_MinJumpHeight;
	FVector CollisionExtentAttachFactorRef = m_CollisionExtentAttachFactor;

	if (CVarOverrideJumpAttach.GetValueOnGameThread() > 0)
	{
		TargetHeightFactorRef = CVarJumpAttachHeightFactor.GetValueOnGameThread();
		TargetHeightOffsetRef = CVarJumpAttachHeightOffset.GetValueOnGameThread();
		TargetMinJumpHeightRef = CVarJumpAttachMinJumpHeight.GetValueOnGameThread();
		CollisionExtentAttachFactorRef.X = CVarJumpAttachCollisionExtentFactorX.GetValueOnGameThread();
		CollisionExtentAttachFactorRef.Y = CVarJumpAttachCollisionExtentFactorY.GetValueOnGameThread();
		CollisionExtentAttachFactorRef.Z = CVarJumpAttachCollisionExtentFactorZ.GetValueOnGameThread();
	}
#else
	const float& TargetHeightFactorRef = m_TargetHeightFactor;
	const float& TargetHeightOffsetRef = m_TargetHeightOffset;
	const float& TargetMinJumpHeightRef = m_MinJumpHeight;
	const FVector& CollisionExtentAttachFactorRef = m_CollisionExtentAttachFactor;
#endif// !UE_BUILD_SHIPPING

	TSharedPtr<FSPRootMotionSource_JumpForce> JumpForce = MakeShared<FSPRootMotionSource_JumpForce>();
	JumpForce->InstanceName = FName(TEXT("SPJumpAttachRootMotionTask"));
	JumpForce->AccumulateMode = ERootMotionAccumulateMode::Override;
	JumpForce->Priority = 500;
	JumpForce->Duration = GetDuration();
	JumpForce->bDisableTimeout = false;
	JumpForce->PathOffsetCurve = m_PathOffsetCurve.LoadSynchronous();
	JumpForce->TimeMappingCurve = m_TimeMappingCurve.LoadSynchronous();
	JumpForce->FinishVelocityParams.Mode = ERootMotionFinishVelocityMode::SetVelocity;
	JumpForce->FinishVelocityParams.SetVelocity = FVector::ZeroVector;
	JumpForce->FinishVelocityParams.ClampVelocity = 0.f;
	// ****** 计算跳跃表现需要的参数
	const bool OwnerIsAttachedToTargetActor = OwnerPawn->GetAttachParentActor() == TargetMeshComponent->GetOwner();
	const FTransform& TargetActorWorldTrans = TargetMeshComponent->GetOwner()->GetActorTransform();
	// *** 计算朝向
	// Owner的碰撞体大小
	FVector OwnerCollisionExtent(EForceInit::ForceInitToZero);
	FVector OwnerPawnToAttachPointVector(EForceInit::ForceInitToZero);
	FVector OwnerAttachLocationOffset = CalculateOwnerAttachLocationOffset(
		*OwnerMoveUpdateComp,
		CollisionExtentAttachFactorRef,
		&OwnerCollisionExtent);
	bool UseExteralLocations = false;

	if (m_StartLocInTargetActorSpaceParamName != NAME_None && m_AttachLocInTargetActorSpaceParamName != NAME_None)
	{
		const FVector* StartLocInTargetActorSpacePtr = Context->GetVectorParameters().Find(m_StartLocInTargetActorSpaceParamName);
		const FVector* AttachLocInTargetActorSpacePtr = Context->GetVectorParameters().Find(m_AttachLocInTargetActorSpaceParamName);

		if (StartLocInTargetActorSpacePtr && AttachLocInTargetActorSpacePtr)
		{
			UseExteralLocations = true;

			FTransform AttachPointTransInTargetActorSpace = TargetMeshComponent->GetSocketTransform(SocketName, ERelativeTransformSpace::RTS_Actor);
			AttachPointTransInTargetActorSpace.SetLocation(*AttachLocInTargetActorSpacePtr);
			OwnerPawnToAttachPointVector = AttachPointTransInTargetActorSpace.TransformPosition(OwnerAttachLocationOffset) - *StartLocInTargetActorSpacePtr;

			if (!OwnerIsAttachedToTargetActor)
			{
				OwnerPawnToAttachPointVector = TargetActorWorldTrans.TransformVector(OwnerPawnToAttachPointVector);
			}
		}
	}
	
	if (!UseExteralLocations)
	{
		const FVector OwnerPawnWorldLoc = OwnerPawn->GetActorLocation();

		if (CollisionExtentAttachFactorRef.IsZero())
		{
			FTransform AttachPointWorldTrans = TargetMeshComponent->GetSocketTransform(SocketName, ERelativeTransformSpace::RTS_World);
			OwnerPawnToAttachPointVector = AttachPointWorldTrans.TransformPosition(OwnerAttachLocationOffset) - OwnerPawnWorldLoc;
		}
		else
		{
			OwnerPawnToAttachPointVector = TargetMeshComponent->GetSocketLocation(SocketName) - OwnerPawnWorldLoc;
		}

		if (OwnerIsAttachedToTargetActor)
		{
			OwnerPawnToAttachPointVector = TargetActorWorldTrans.InverseTransformVector(OwnerPawnToAttachPointVector);
		}
	}

	JumpForce->Rotation = OwnerPawnToAttachPointVector.GetSafeNormal().Rotation();
	// *** 计算高度
	// 目标挂点在Actor空间的位置
	// 目标碰撞体高度
	const FVector TargetCollisionExtent = TargetMoveUpdateComp->GetCollisionShape().GetExtent();
	const float JumpHeightOffsetInActorSpace = (TargetCollisionExtent.Z + TargetCollisionExtent.Z) * TargetHeightFactorRef + TargetHeightOffsetRef;
	const float JumpHeightInOwnerSpace = FMath::Max(FMath::Max(TargetMinJumpHeightRef, 1.f), OwnerPawnToAttachPointVector.Z + JumpHeightOffsetInActorSpace);
	JumpForce->Height = JumpHeightInOwnerSpace;
	// *** 计算跳跃曲线终点的X轴值
	// 计算Y轴值
	const float JumpAxisY = OwnerPawnToAttachPointVector.Z / JumpHeightInOwnerSpace;
	// 计算X轴值
	const float JumpAxisX = (1.f + FMath::Sqrt(1.f - JumpAxisY)) * 0.5f;
	JumpForce->EndFraction = JumpAxisX;
	// 计算跳跃曲线X为1时的距离
	JumpForce->Distance = OwnerPawnToAttachPointVector.Size2D() / JumpAxisX;
	// ****** 添加RootMotionSource 
	ScratchPad->RootMotionSourceID = MoveComp->ApplyRootMotionSource(JumpForce);
	// ****** 记录临时变量
	// 记录跳跃最高点的时间
	ScratchPad->JumpPeakTime = GetStartTime() + GetDuration() *  0.5f / JumpAxisX;
	ScratchPad->OldNetSmoothMode = MoveComp->NetworkSmoothingMode;
	ScratchPad->TargetMeshCompPtr = TargetMeshComponent;
	ScratchPad->OldMeshParentCompPtr = OwnerMeshComp ? OwnerMeshComp->GetAttachParent() : nullptr;
	// 记录Owner模型的默认相对旋转
	const ACharacter* OwnerCDO = OwnerPawn->GetClass() ? Cast<ACharacter>(OwnerPawn->GetClass()->GetDefaultObject()) : nullptr;
	const USkeletalMeshComponent* OwnerCDOMeshComp = OwnerCDO ? OwnerCDO->GetMesh() : nullptr;

	if (OwnerCDOMeshComp)
	{
		ScratchPad->DefaultOwnerMeshQuat = OwnerCDOMeshComp->GetRelativeRotation().Quaternion();
	}
	else
	{
		ScratchPad->DefaultOwnerMeshQuat = FQuat::Identity;
	}
	// 记录Owner模型的默认相对位置
	ScratchPad->DefaultOwnerMeshLoc = FVector(0.f, 0.f, -OwnerCollisionExtent.Z);
}


void USPJumpAttachRootMotionTask::OnTaskTickBP_Implementation(const UAbleAbilityContext* Context, float DeltaTime) const
{
	check(Context);

	ACharacter* OwnerPawn = Cast<ACharacter>(Context->GetOwner());

	if (!OwnerPawn)
	{
		return;
	}

	USPJumpAttachRootMotionTaskScratchPad* ScratchPad = Cast<USPJumpAttachRootMotionTaskScratchPad>(GetScratchPad(const_cast<UAbleAbilityContext*>(Context)));

	if (!ScratchPad)
	{
		return;
	}

	UPrimitiveComponent* OwnerMoveUpdateComp = Cast<UPrimitiveComponent>(OwnerPawn->GetRootComponent());
	UPrimitiveComponent* TargetMeshComp = ScratchPad->TargetMeshCompPtr.Get();

	if (!OwnerMoveUpdateComp || !TargetMeshComp || !TargetMeshComp->GetOwner())
	{
		return;
	}

	const float RotationPassTime = Context->GetCurrentTime() - GetStartTime() - m_StartRotationTime;
	const float RotationLeftTime = GetDuration() - m_StartRotationTime;

	if (RotationPassTime > 0.f && RotationLeftTime > SMALL_NUMBER)
	{
		const FTransform& TargetRootSpace = TargetMeshComp->GetOwner()->GetActorTransform();
		const float OwnerRotationLerpPercent = RotationPassTime / RotationLeftTime;

		if (!ScratchPad->bRotationLerping)
		{
			ScratchPad->bRotationLerping = true;
			ScratchPad->OwnerJumpStartRotationInTargetRootSpace = TargetRootSpace.InverseTransformRotation(OwnerPawn->GetActorQuat());
		}

		const FQuat SmoothedOwnerRotation = FQuat::Slerp(
			TargetRootSpace.TransformRotation(ScratchPad->OwnerJumpStartRotationInTargetRootSpace),
			TargetMeshComp->GetOwner()->GetActorQuat(),
			OwnerRotationLerpPercent).GetNormalized();
		OwnerPawn->SetActorRotation(SmoothedOwnerRotation);
	}

	if (!USPUnifiedNetworkLibrary::CanPlayEffect(OwnerPawn))
	{
		return;
	}

	const float PassPeakTime = Context->GetCurrentTime() - ScratchPad->JumpPeakTime;

	if (PassPeakTime < 0.f)
	{
		return;
	}

#if !UE_BUILD_SHIPPING
	float AttachRadiusSquaredRef = m_AttachRadiusSquared;
	FVector CollisionExtentAttachFactorRef = m_CollisionExtentAttachFactor;

	if (CVarOverrideJumpAttach.GetValueOnGameThread() > 0)
	{
		AttachRadiusSquaredRef = CVarJumpAttachRadiusSquared.GetValueOnGameThread();
	}
#else
	const float& AttachRadiusSquaredRef = m_AttachRadiusSquared;
	const FVector& CollisionExtentAttachFactorRef = m_CollisionExtentAttachFactor;
#endif// !UE_BUILD_SHIPPING

	FVector OwnerAttachLocationOffset = CalculateOwnerAttachLocationOffset(*OwnerMoveUpdateComp, CollisionExtentAttachFactorRef);
	const FTransform& AttachPointWorldTrans = TargetMeshComp->GetSocketTransform(ScratchPad->SocketName, ERelativeTransformSpace::RTS_World);
	FVector OwnerAttachPointWorldLoc = AttachPointWorldTrans.TransformPosition(OwnerAttachLocationOffset);
	const FVector OwnerWorldLoc = OwnerPawn->GetActorLocation();

	if ((OwnerWorldLoc - OwnerAttachPointWorldLoc).SizeSquared() > AttachRadiusSquaredRef)
	{
		return;
	}

	USkeletalMeshComponent* OwnerMeshComp = OwnerPawn->GetMesh();
	UCharacterMovementComponent* MoveComp = OwnerPawn->GetCharacterMovement();
	USceneComponent* TargetOwnerRootComp = TargetMeshComp->GetOwner()->GetRootComponent();

	if (!MoveComp || !OwnerMeshComp || !TargetOwnerRootComp)
	{
		return;
	}

	FScopedMovementUpdate ScopedUpdate(OwnerMeshComp, EScopedUpdate::DeferredUpdates);

	if (!ScratchPad->bAttachLerping)
	{
		ScratchPad->bAttachLerping = true;

		if (OwnerPawn->GetLocalRole() == ROLE_SimulatedProxy)
		{
			MoveComp->NetworkSmoothingMode = ENetworkSmoothingMode::Disabled;
		}

		if (OwnerMeshComp->GetAttachParent() != TargetOwnerRootComp)
		{
			OwnerMeshComp->AttachToComponent(
				TargetOwnerRootComp,
				FAttachmentTransformRules::KeepWorldTransform);
		}

		ScratchPad->LerpEndOwnerMeshLoc = OwnerAttachLocationOffset + ScratchPad->DefaultOwnerMeshLoc;
		ScratchPad->LerpEndOwnerMeshQuat = ScratchPad->DefaultOwnerMeshQuat;
	}

	// 对mesh进行插值
	float LerpDuration = GetEndTime() - ScratchPad->JumpPeakTime;
	float LerpPercent = LerpDuration > SMALL_NUMBER ? (PassPeakTime / LerpDuration) : 1.f;

	if (IsValid(m_AttachLerpingCurve.LoadSynchronous()))
	{
		LerpPercent = m_AttachLerpingCurve->GetFloatValue(LerpPercent);
	}

	const FVector& MeshWorldScale = OwnerMeshComp->GetComponentScale();
	const FTransform MeshLocalTrans(ScratchPad->DefaultOwnerMeshQuat, ScratchPad->DefaultOwnerMeshLoc);
	FVector SmoothedMoveUpdateWorldLoc = FMath::LerpStable(OwnerWorldLoc, OwnerAttachPointWorldLoc, LerpPercent);
	FQuat SmoothedMoveUpdateWorldQuat = FQuat::Slerp(OwnerPawn->GetActorQuat(), AttachPointWorldTrans.GetRotation(), LerpPercent).GetNormalized();
	FTransform SmoothedMeshTransInWorldSpace = MeshLocalTrans *
		FTransform(SmoothedMoveUpdateWorldQuat, SmoothedMoveUpdateWorldLoc, MeshWorldScale);
	OwnerMeshComp->SetWorldLocationAndRotation(
		SmoothedMeshTransInWorldSpace.GetLocation(),
		SmoothedMeshTransInWorldSpace.GetRotation(),
		false,
		nullptr,
		ETeleportType::TeleportPhysics);

#if !UE_BUILD_SHIPPING
	if (CVarDebugJumpAttachLerping.GetValueOnGameThread() > 0)
	{
		const FVector DrawedMeshLoc = OwnerMeshComp->GetComponentTransform().TransformPosition(-ScratchPad->LerpEndOwnerMeshLoc);
		DrawDebugCoordinateSystem(
			OwnerMeshComp->GetWorld(),
			DrawedMeshLoc,
			OwnerMeshComp->GetComponentTransform().TransformRotation(ScratchPad->LerpEndOwnerMeshQuat.Inverse()).Rotator(),
			100.f, false, 6.f, 0u, 1.f);
		DrawDebugString(
			OwnerMeshComp->GetWorld(),
			DrawedMeshLoc + FVector(10.f, 10.f, 20.f),
			FString::Printf(TEXT("Lerp:%.3f"), LerpPercent),
			OwnerPawn,
			FColor::Yellow, 6.f, false, 10.f);
	}
#endif// !UE_BUILD_SHIPPING
}


void USPJumpAttachRootMotionTask::OnTaskEndBP_Implementation(const UAbleAbilityContext* Context, const EAbleAbilityTaskResult result) const
{
	check(Context);

	USPJumpAttachRootMotionTaskScratchPad* ScratchPad = Cast<USPJumpAttachRootMotionTaskScratchPad>(GetScratchPad(const_cast<UAbleAbilityContext*>(Context)));

	if (!ScratchPad)
	{
		return;
	}

	ACharacter* OwnerPawn = Cast<ACharacter>(Context->GetOwner());

	if (OwnerPawn)
	{
		UCharacterMovementComponent* MoveComp = OwnerPawn->GetCharacterMovement();

		if (MoveComp)
		{
			MoveComp->RemoveRootMotionSourceByID(ScratchPad->RootMotionSourceID);
			MoveComp->NetworkSmoothingMode = ScratchPad->OldNetSmoothMode;
		}

		if (ScratchPad->bAttachLerping)
		{
			USceneComponent* OldMeshParentComp = ScratchPad->OldMeshParentCompPtr.Get();
			USkeletalMeshComponent* OwnerMeshComp = OwnerPawn->GetMesh();

			if (OwnerMeshComp && OwnerMeshComp->GetAttachParent() != OldMeshParentComp)
			{
				if (OldMeshParentComp)
				{
					OwnerMeshComp->AttachToComponent(
						OldMeshParentComp,
						FAttachmentTransformRules::KeepWorldTransform);
				}
				else
				{
					OwnerMeshComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
				}
			}
		}
	}

	ScratchPad->ResetPad();
}


USPAbilityTaskScratchPad* USPJumpAttachRootMotionTask::CreateScratchPadBP_Implementation(UAbleAbilityContext* Context) const
{
	if (UAbleAbilityUtilitySubsystem* Subsystem = Context->GetUtilitySubsystem())
	{
		static TSubclassOf<UAbleAbilityTaskScratchPad> ScratchPadClass = USPJumpAttachRootMotionTaskScratchPad::StaticClass();
		return Cast<USPAbilityTaskScratchPad>(Subsystem->FindOrConstructTaskScratchPad(ScratchPadClass));
	}

	return NewObject<USPJumpAttachRootMotionTaskScratchPad>(Context);
}


TStatId USPJumpAttachRootMotionTask::GetStatId() const
{
	return GET_STATID(STAT_SPJumpAttachRootMotionTask);
}

#if WITH_EDITOR

FText USPJumpAttachRootMotionTask::GetDescriptiveTaskNameBP_Implementation() const
{
	const FText FormatText = LOCTEXT("SPJumpAttachRootMotionTaskFormat", "{0}: {1}");
	return FText::FormatOrdered(FormatText, GetTaskName(), FText::FromName(m_SocketName));
}

#endif

FVector USPJumpAttachRootMotionTask::CalculateOwnerAttachLocationOffset(
	const UPrimitiveComponent& InOwnerMoveUpdateCompRef,
	const FVector& InCollisionExtentAttachFactor,
	FVector* IoOwnerCollisionExtentPtr)
{
	const FVector OwnerCollisionExtent = InOwnerMoveUpdateCompRef.GetCollisionShape().GetExtent();

	if (IoOwnerCollisionExtentPtr)
	{
		*IoOwnerCollisionExtentPtr = OwnerCollisionExtent;
	}

	return InCollisionExtentAttachFactor * OwnerCollisionExtent;
}

#undef LOCTEXT_NAMESPACE

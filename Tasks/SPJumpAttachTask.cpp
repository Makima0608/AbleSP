#include "Game/SPGame/Skill/Task/SPJumpAttachTask.h"

#include "Stats/Stats2.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"

#include "ableSubSystem.h"

#if !(UE_BUILD_SHIPPING)
#include "ableAbilityUtilities.h"
#endif

#include "Game/SPGame/Common/JumpAttach/SPJumpAttachConfig.h"
#include "Game/SPGame/Common/JumpAttach/SPJumpAttachAnimMetaData.h"
#include "Game/SPGame/Common/JumpAttach/SPJumpAttachFlow.h"



DECLARE_STATS_GROUP(TEXT("SPJumpAttachAbility"), STATGROUP_SPJumpAttachAbility, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("SPJumpAttachTask"), STAT_SPJumpAttachTask, STATGROUP_SPJumpAttachAbility);

#define LOCTEXT_NAMESPACE "SPAbleAbilityTask"


USPJumpAttachTaskScratchPad::USPJumpAttachTaskScratchPad()
	: FlowLogic(nullptr)
	, FlowData(nullptr)
{
}


void USPJumpAttachTaskScratchPad::ResetPad_Implementation()
{
	FlowLogic = nullptr;

	if (IsValid(FlowData) && FlowData->GetOuter() == this)
	{
		FlowData->MarkPendingKill();
	}

	FlowData = nullptr;
}


bool USPJumpAttachTaskScratchPad::HasFlowStarted() const
{
	return IsValid(FlowLogic) && IsValid(FlowData) && FlowLogic->HasLogicStarted(*FlowData);
}


USPJumpAttachTask::USPJumpAttachTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, m_JumpAttachConfig(nullptr)
{
}


FString USPJumpAttachTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPJumpAttachTask");
}


void USPJumpAttachTask::OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const
{
	check(Context);

	USPJumpAttachTaskScratchPad* ScratchPad = Cast<USPJumpAttachTaskScratchPad>(GetScratchPad(const_cast<UAbleAbilityContext*>(Context)));

	if (!ScratchPad)
	{
		return;
	}

	FSPJumpAttachFlowParams FlowParams;

	if (!CreateFlowParams(FlowParams, *ScratchPad, *Context))
	{
		return;
	}

	USPJumpAttachFlowLogic* FlowLogic = nullptr;
	USPJumpAttachFlowData* FlowData = nullptr;

	if (!m_JumpAttachFlowLogicParamName.IsNone())
	{
		if (auto FoundParamPtr = Context->GetUObjectParameters().Find(m_JumpAttachFlowLogicParamName))
		{
			FlowLogic = Cast<USPJumpAttachFlowLogic>(*FoundParamPtr);
		}
	}

	if (!IsValid(FlowLogic))
	{
		FlowLogic = GetMutableDefault<USPJumpAttachFlowLogic>();
	}

	if (!FlowLogic)
	{
		return;
	}

	if (!m_JumpAttachFlowDataParamName.IsNone())
	{
		if (auto FoundParamPtr = Context->GetUObjectParameters().Find(m_JumpAttachFlowDataParamName))
		{
			FlowData = Cast<USPJumpAttachFlowData>(*FoundParamPtr);
		}
	}

	if (!IsValid(FlowData))
	{
		FlowData = NewObject<USPJumpAttachFlowData>(ScratchPad);
	}

	if (!FlowData)
	{
		return;
	}

	ScratchPad->FlowLogic = FlowLogic;
	ScratchPad->FlowData = FlowData;

	if (!FlowLogic->TryStartLogic(*FlowData, FlowParams))
	{
		return;
	}
}


void USPJumpAttachTask::OnTaskTickBP_Implementation(const UAbleAbilityContext* Context, float DeltaTime) const
{
	check(Context);

	USPJumpAttachTaskScratchPad* ScratchPad = Cast<USPJumpAttachTaskScratchPad>(GetScratchPad(const_cast<UAbleAbilityContext*>(Context)));

	if (!ScratchPad || !ScratchPad->HasFlowStarted())
	{
		return;
	}

	const float ElapsedTime = Context->GetCurrentTime() - GetStartTime();
	ScratchPad->FlowLogic->UpdateLogic(*ScratchPad->FlowData, ElapsedTime, DeltaTime);
}


void USPJumpAttachTask::OnTaskEndBP_Implementation(const UAbleAbilityContext* Context, const EAbleAbilityTaskResult result) const
{
	check(Context);

	USPJumpAttachTaskScratchPad* ScratchPad = Cast<USPJumpAttachTaskScratchPad>(GetScratchPad(const_cast<UAbleAbilityContext*>(Context)));

	if (!ScratchPad || !ScratchPad->HasFlowStarted())
	{
		return;
	}

	ScratchPad->FlowLogic->EndLogic(*ScratchPad->FlowData, true);
	ScratchPad->ResetPad();
}


bool USPJumpAttachTask::IsDoneBP_Implementation(const UAbleAbilityContext* Context) const
{
	if (!Context)
	{
		return true;
	}

	USPJumpAttachTaskScratchPad* ScratchPad = Cast<USPJumpAttachTaskScratchPad>(GetScratchPad(const_cast<UAbleAbilityContext*>(Context)));

	if (!ScratchPad || !ScratchPad->HasFlowStarted())
	{
		return Context->GetCurrentTime() >= GetEndTime();
	}

	return Context->GetCurrentTime() - GetStartTime() >= ScratchPad->FlowData->Duration;
}


USPAbilityTaskScratchPad* USPJumpAttachTask::CreateScratchPadBP_Implementation(UAbleAbilityContext* Context) const
{
	check(Context);

	if (UAbleAbilityUtilitySubsystem* Subsystem = Context->GetUtilitySubsystem())
	{
		static TSubclassOf<UAbleAbilityTaskScratchPad> ScratchPadClass = USPJumpAttachTaskScratchPad::StaticClass();
		return Cast<USPAbilityTaskScratchPad>(Subsystem->FindOrConstructTaskScratchPad(ScratchPadClass));
	}

	return NewObject<USPJumpAttachTaskScratchPad>(Context);
}


TStatId USPJumpAttachTask::GetStatId() const
{
	return GET_STATID(STAT_SPJumpAttachTask);
}

#if WITH_EDITOR

FText USPJumpAttachTask::GetDescriptiveTaskNameBP_Implementation() const
{
	const FText FormatText = LOCTEXT("SPJumpAttachTaskFormat", "{0}");
	return FText::FormatOrdered(FormatText, GetTaskName());
}

#endif

bool USPJumpAttachTask::CreateFlowParams(
	FSPJumpAttachFlowParams& OutParams,
	USPAbilityTaskScratchPad& IoScratchPadRef,
	const UAbleAbilityContext& InContextRef) const
{
	bool  ParamIsNotFound = true;
	USceneComponent* JumpSpaceComp = nullptr;

	if (!m_JumpSpaceCompParamName.IsNone())
	{
		if (auto FoundParamPtr = InContextRef.GetUObjectParameters().Find(m_JumpSpaceCompParamName))
		{
			JumpSpaceComp = Cast<USceneComponent>(*FoundParamPtr);
			ParamIsNotFound = false;
		}
	}

	if (ParamIsNotFound && InContextRef.GetTargetActors().Num() > 0)
	{
		AActor* FirstTargetActor = Cast<ACharacter>(InContextRef.GetTargetActors()[0]);

		if (IsValid(FirstTargetActor))
		{
			JumpSpaceComp = FirstTargetActor->GetRootComponent();
		}
	}

	if (!IsValid(JumpSpaceComp))
	{
		return false;
	}

	ACharacter* JumpSpaceCharacter = Cast<ACharacter>(JumpSpaceComp->GetOwner());
	USceneComponent* TargetParentComp = nullptr;
	ParamIsNotFound = true;

	if (!m_JumpTargetParentCompParamName.IsNone())
	{
		if (auto FoundParamPtr = InContextRef.GetUObjectParameters().Find(m_JumpTargetParentCompParamName))
		{
			TargetParentComp = Cast<USceneComponent>(*FoundParamPtr);
			ParamIsNotFound = false;
		}
	}
	
	if (ParamIsNotFound && IsValid(JumpSpaceCharacter))
	{
		TargetParentComp = JumpSpaceCharacter->GetMesh();
	}

	if (!IsValid(TargetParentComp) || !IsValid(TargetParentComp->GetOwner()))
	{
		return false;
	}

	ACharacter* OwnerCharacter = nullptr;
	USceneComponent* MovedComp = nullptr;
	ParamIsNotFound = true;

	if (!m_JumpMovedCompParamName.IsNone())
	{
		if (auto FoundParamPtr = InContextRef.GetUObjectParameters().Find(m_JumpMovedCompParamName))
		{
			MovedComp = Cast<USceneComponent>(*FoundParamPtr);
			ParamIsNotFound = false;

			if (MovedComp)
			{
				OwnerCharacter = Cast<ACharacter>(MovedComp->GetOwner());
			}
		}
	}
	
	if (ParamIsNotFound)
	{
		OwnerCharacter = Cast<ACharacter>(InContextRef.GetOwner());

		if (IsValid(OwnerCharacter))
		{
			MovedComp = OwnerCharacter->GetMesh();
		}
	}

	if (!IsValid(MovedComp))
	{
		return false;
	}

	USceneComponent* MoveRootComp = nullptr;
	ParamIsNotFound = true;

	if (!m_JumpMoveRootCompParamName.IsNone())
	{
		if (auto FoundParamPtr = InContextRef.GetUObjectParameters().Find(m_JumpMoveRootCompParamName))
		{
			MoveRootComp = Cast<USceneComponent>(*FoundParamPtr);
			ParamIsNotFound = false;
		}
	}
	
	if (ParamIsNotFound && IsValid(OwnerCharacter))
	{
		MoveRootComp = OwnerCharacter->GetRootComponent();
	}

	if (!IsValid(MoveRootComp))
	{
		return false;
	}

	USPJumpAttachConfig* JumpAttachConfig = nullptr;

	if (!m_JumpAttachConfigParamName.IsNone())
	{
		if (auto FoundParamPtr = InContextRef.GetUObjectParameters().Find(m_JumpAttachConfigParamName))
		{
			JumpAttachConfig = Cast<USPJumpAttachConfig>(*FoundParamPtr);
		}
	}
	else
	{
		JumpAttachConfig = m_JumpAttachConfig;
	}

	if (!IsValid(JumpAttachConfig))
	{
		JumpAttachConfig = GetMutableDefault<USPJumpAttachConfig>();
	}
	
	if (!JumpAttachConfig)
	{
		return false;
	}

	UAnimSequenceBase* JumpAnim = nullptr;

	if (!m_JumpAnimParamName.IsNone())
	{
		if (auto FoundParamPtr = InContextRef.GetUObjectParameters().Find(m_JumpAnimParamName))
		{
			JumpAnim = Cast<UAnimSequenceBase>(*FoundParamPtr);
		}
	}

	float AnimPlayRate = 1.f;

	if (!m_JumpAnimPlayRateParamName.IsNone())
	{
		if (auto FoundParamPtr = InContextRef.GetFloatParameters().Find(m_JumpAnimPlayRateParamName))
		{
			AnimPlayRate = *FoundParamPtr;
		}
	}

	float Duration = 0.f;
	ParamIsNotFound = true;

	if (!m_JumpDurationParamName.IsNone())
	{
		if (auto FoundParamPtr = InContextRef.GetFloatParameters().Find(m_JumpDurationParamName))
		{
			Duration = *FoundParamPtr;
			ParamIsNotFound = false;
		}
	}

	if (ParamIsNotFound)
	{
		if (IsValid(JumpAnim))
		{
			Duration = const_cast<UAnimSequenceBase*>(JumpAnim)->GetPlayLength() * AnimPlayRate;
		}
		else
		{
			Duration = GetDuration() * AnimPlayRate;
		}
	}

	if (Duration < SMALL_NUMBER)
	{
		return false;
	}

	OutParams.JumpSpaceComp = JumpSpaceComp;
	OutParams.MovedComp = MovedComp;
	OutParams.TargetParentComp = TargetParentComp;
	OutParams.MoveRootComp = MoveRootComp;
	OutParams.JumpAttachConfig = JumpAttachConfig;
	OutParams.Duration = Duration;
	OutParams.JumpAnim = JumpAnim;
	OutParams.AnimPlayRate = AnimPlayRate;

	if (!m_JumpStartLocInJumpSpaceParamName.IsNone())
	{
		if (auto FoundParamPtr = InContextRef.GetVectorParameters().Find(m_JumpStartLocInJumpSpaceParamName))
		{
			OutParams.JumpStartLocInJumpSpace = (*FoundParamPtr);
			OutParams.bUseParamJumpStartLocInJumpSpace = true;
		}
	}

	if (!m_JumpPeakZInJumpSpaceParamName.IsNone())
	{
		if (auto FoundParamPtr = InContextRef.GetFloatParameters().Find(m_JumpPeakZInJumpSpaceParamName))
		{
			OutParams.JumpPeakZInJumpSpace = *FoundParamPtr;
			OutParams.bUseParamJumpPeakZInJumpSpace = true;
		}
	}

	if (!m_JumpStartRotInJumpSpaceParamName.IsNone())
	{
		if (auto FoundParamPtr = InContextRef.GetVectorParameters().Find(m_JumpStartRotInJumpSpaceParamName))
		{
			const FVector& StartRotVectorParam = *FoundParamPtr;
			OutParams.JumpStartQuatInJumpSpace = FRotator(StartRotVectorParam.X, StartRotVectorParam.Y, StartRotVectorParam.Z).Quaternion();
			OutParams.bUseParamJumpStartQuatInJumpSpace = true;
		}
	}

	if (!m_JumpAttachSocketNameParamName.IsNone())
	{
		if (auto FoundParamPtr = InContextRef.GetStringParameters().Find(m_JumpAttachSocketNameParamName))
		{
			OutParams.AttachSocketName = FName(**FoundParamPtr);
		}
	}

	if (!m_JumpAttachOffsetLocParamName.IsNone())
	{
		if (auto FoundParamPtr = InContextRef.GetVectorParameters().Find(m_JumpAttachOffsetLocParamName))
		{
			OutParams.AttachOffsetLoc = (*FoundParamPtr);
			OutParams.bUseParamJumpAttachOffsetLoc = true;
		}
	}

	if (!m_JumpEndLocInJumpSpaceParamName.IsNone())
	{
		if (auto FoundParamPtr = InContextRef.GetVectorParameters().Find(m_JumpEndLocInJumpSpaceParamName))
		{
			OutParams.JumpEndLocInJumpSpace = (*FoundParamPtr);
			OutParams.bUseParamJumpEndLocInJumpSpace = true;
		}
	}

	FQuat JumpEndQuatInJumpSpace = FQuat::Identity;
	
	if (!m_JumpEndRotInJumpSpaceParamName.IsNone())
	{
		if (auto FoundParamPtr = InContextRef.GetVectorParameters().Find(m_JumpEndRotInJumpSpaceParamName))
		{
			const FVector& EndRotVectorParam = *FoundParamPtr;
			OutParams.JumpEndQuatInJumpSpace = FRotator(EndRotVectorParam.X, EndRotVectorParam.Y, EndRotVectorParam.Z).Quaternion();
			OutParams.bUseParamJumpEndQuatInJumpSpace = true;
		}
	}

	USPJumpAttachAnimMetaData* JumpAttachAnimMetaData = nullptr;

	if (!m_JumpAttachAnimMetaDataParamName.IsNone())
	{
		if (auto FoundParamPtr = InContextRef.GetUObjectParameters().Find(m_JumpAttachAnimMetaDataParamName))
		{
			OutParams.JumpAttachAnimMetaData = Cast<USPJumpAttachAnimMetaData>(*FoundParamPtr);
			OutParams.bUseParamJumpAttachAnimMetaData = true;
		}
	}

	return true;
}

#undef LOCTEXT_NAMESPACE

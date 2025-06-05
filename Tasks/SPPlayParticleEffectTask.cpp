#include "Game/SPGame/Skill/Task/SPPlayParticleEffectTask.h"

#include "ableAbility.h"
#include "ableSubSystem.h"
#include "AkGameplayStatics.h"
#include "WwiseItemType.h"
#include "Game/SPGame/Utils/SPGameLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/MoeSoundManager.h"
#include "Sound/MoeSoundManagerUtility.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"


USPPlayParticleEffectTaskScratchPad::USPPlayParticleEffectTaskScratchPad()
{
}

USPPlayParticleEffectTaskScratchPad::~USPPlayParticleEffectTaskScratchPad()
{
}

USPPlayParticleEffectTask::USPPlayParticleEffectTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), m_Target(ATT_Self), m_UnLoadCD(0)
{
}

USPPlayParticleEffectTask::~USPPlayParticleEffectTask()
{
}

FString USPPlayParticleEffectTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPPlayParticleEffectTask");
}

void USPPlayParticleEffectTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);
	OnTaskStartBP_Override(Context.Get());
}

void USPPlayParticleEffectTask::OnTaskStartBP_Override_Implementation(const UAbleAbilityContext* Context) const
{
	if (!FApp::IsGame())
	{
		DefaultPlaySound(Context);
	}
	else
	{
		UMoeSoundManager* MoeSoundManager = UMoeSoundManagerUtility::GetSoundManager(Context->GetSelfActor());
		if (USPPlayParticleEffectTaskScratchPad* ScratchPad = Cast<USPPlayParticleEffectTaskScratchPad>(
			Context->GetScratchPadForTask(this)))
		{
			for (const auto SpawnedEffect : ScratchPad->SpawnedEffects)
			{
				ScratchPad->m_EffectsAndPlayingID.Add(SpawnedEffect->GetName(),
				                                      MoeSoundManager->DoPlaySoundAtPosition(
					                                      m_BankName, m_PlayEvent,
					                                      SpawnedEffect->GetComponentLocation(),
					                                      SpawnedEffect->GetComponentRotation()));
			}
		}
		else
		{
			DefaultPlaySound(Context);
		}
	}
}

void USPPlayParticleEffectTask::OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
                                           float deltaTime) const
{
	Super::OnTaskTick(Context, deltaTime);
	OnTaskTickBP_Override(Context.Get(), deltaTime);
}

void USPPlayParticleEffectTask::OnTaskTickBP_Override_Implementation(const UAbleAbilityContext* Context,
	float deltaTime) const
{
}

void USPPlayParticleEffectTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,
                                          const EAbleAbilityTaskResult result) const
{
    OnTaskEndBP_Override(Context.Get(), result);
	Super::OnTaskEnd(Context, result);
}

void USPPlayParticleEffectTask::OnTaskEndBP_Override_Implementation(const UAbleAbilityContext* Context,
	const EAbleAbilityTaskResult result) const
{
    if (!FApp::IsGame())
	{
		AActor* AnimActor = GetSingleActorFromTargetType(Context, m_Target);
		if (!AnimActor) return;
		USceneComponent* RootComp = AnimActor->GetRootComponent();
		if (!RootComp) return;
		bool ComponentCreated = false;
		UAkComponent* AkComp = UAkGameplayStatics::GetAkComponent(RootComp, ComponentCreated, NAME_None,
		                                                          FVector::ZeroVector,
		                                                          EAttachLocation::KeepRelativeOffset);
		if (AkComp)
		{
			AkComp->Stop();
		}
	}
	else
	{
		if (!IsSingleFrame())
		{
			if (USPPlayParticleEffectTaskScratchPad* ScratchPad = Cast<USPPlayParticleEffectTaskScratchPad>(
				Context->GetScratchPadForTask(this)))
			{
				UMoeSoundManager* MoeSoundManager = UMoeSoundManagerUtility::GetSoundManager(Context->GetSelfActor());
				for (auto SpawnedEffect : ScratchPad->SpawnedEffects)
				{
					UParticleSystemComponent* ParticleSystemComponent = SpawnedEffect.Get();
					FString KeyName = SpawnedEffect->GetName();
					MoeSoundManager->StopSoundAtPostion(ParticleSystemComponent,
					                                    ScratchPad->m_EffectsAndPlayingID[KeyName], false,
					                                    m_PlayEvent,
					                                    m_StopEvent, m_BankName, m_UnLoadCD, false);
				}
			}
		}
	}
}

bool USPPlayParticleEffectTask::IsDone(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	return UAbleAbilityTask::IsDone(Context);
}

UAbleAbilityTaskScratchPad* USPPlayParticleEffectTask::CreateScratchPad(
	const TWeakObjectPtr<UAbleAbilityContext>& Context) const
{
	if (m_DestroyAtEnd)
	{
		if (UAbleAbilityUtilitySubsystem* Subsystem = Context->GetUtilitySubsystem())
		{
			static TSubclassOf<UAbleAbilityTaskScratchPad> ScratchPadClass =
				USPPlayParticleEffectTaskScratchPad::StaticClass();
			return Subsystem->FindOrConstructTaskScratchPad(ScratchPadClass);
		}
		return NewObject<USPPlayParticleEffectTaskScratchPad>(Context.Get());
	}
	return nullptr;
}

void USPPlayParticleEffectTask::DefaultPlaySound(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	AActor* AnimActor = GetSingleActorFromTargetType(Context, m_Target);
	const FString Str = "{UseSkin}";
	const FString DefaultCharacterIdBank = "VO_CommonA";
	FString BankNameSkin = m_BankName.Replace(*Str, *DefaultCharacterIdBank);
	UAkGameplayStatics::LoadAndCheckBankByName(BankNameSkin);
	if (!AnimActor) return;
	USceneComponent* RootComp = AnimActor->GetRootComponent();
	if (!RootComp) return;
	bool ComponentCreated = false;
	UAkComponent* AkComp = UAkGameplayStatics::GetAkComponent(RootComp, ComponentCreated, NAME_None,
	                                                          FVector::ZeroVector,
	                                                          EAttachLocation::KeepRelativeOffset);
	if (AkComp)
	{
		const FString PlayEventSkin = m_PlayEvent.Replace(*Str, *DefaultCharacterIdBank);
		AkComp->PostAkEvent(nullptr, 0, FOnAkPostEventCallback(), TArray<FAkExternalSourceInfo>(), PlayEventSkin);
	}
}

TStatId USPPlayParticleEffectTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPPlayParticleEffectTask, STATGROUP_USPAbility);
}

void USPPlayParticleEffectTask::BindDynamicDelegates(UAbleAbility* Ability)
{
	Super::BindDynamicDelegates(Ability);
}

EAbleAbilityTaskRealm USPPlayParticleEffectTask::GetTaskRealmBP_Implementation() const { return EAbleAbilityTaskRealm::ATR_Client; }

bool USPPlayParticleEffectTask::IsSingleFrameBP_Implementation() const { return !m_DestroyAtEnd; }

// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SPGame/Skill/Task/SPRollDisplacemenTask.h"

#include "ableSubSystem.h"
#include "Game/SPGame/Skill/Task/SPRollDisplacemenTaskScratchPad.h"
#include "Game/SPGame/Character/SPGameCharacterBase.h"
#include "Game/SPGame/Character/SPGameCharInputComponent.h"
#include "Game/SPGame/Utils/SPGameLibrary.h"

#define LOCTEXT_NAMESPACE "SPSkillAbilityTask"

USPRollDisplacemenTask::USPRollDisplacemenTask(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer),
m_IsDuration(false),
//m_SetVelocity(0.f),
//m_AddVelocity(0.f),
//m_RestoreAtEnd(false),
//m_ResetValue(0.f),
m_FireEvent(false),
m_Name(NAME_None),
m_TaskRealm(EAbleAbilityTaskRealm::ATR_Client)
{
	
}

USPRollDisplacemenTask::~USPRollDisplacemenTask()
{
	
}

FString USPRollDisplacemenTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPRollDisplacemenTask");
}

void USPRollDisplacemenTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);
	OnTaskStartBP(Context.Get());
}

void USPRollDisplacemenTask::OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const
{
	USPRollDisplacemenTaskScratchPad* ScratchPad = nullptr;
	
	ASPGameCharacterBase* Player = Cast<ASPGameCharacterBase>(Context->GetOwner());
	if (Player&& !Player->IsPendingKill())
	{
		//Player->GetActorForwardVector()
		UMoeCameraManagerSubSystem* CameraManager = UMoeCameraManagerSubSystem::GetInstance();
		if(CameraManager==nullptr)
		{
			return;
		}
		auto Camera = CameraManager->GetCurrentActiveCamera();
		if(Camera==nullptr)
		{
			return;
		}
		auto ForwardVector = Camera->GetComponentRotation().Quaternion().GetForwardVector();
		auto RightVector = Camera->GetComponentRotation().Quaternion().GetRightVector();
		auto InputComponent = Cast<USPGameCharInputComponent>(Player->GetComponentByClass(USPGameCharInputComponent::StaticClass()));
		if (!InputComponent)
		{
			return;
		}
			FVector ImpulseVector = FVector::OneVector;
			
			auto TouchPadDir  = InputComponent->CacheMoveVector;
			if(TouchPadDir.IsNearlyZero())
			{
				ImpulseVector = Player->GetMesh()->GetRightVector();
			}
			else
			{
				ForwardVector = TouchPadDir.Y * ForwardVector;
				RightVector = TouchPadDir.X * RightVector;
				FVector NewRotation = ForwardVector + RightVector;
				NewRotation.Z = 0;
				
				
				NewRotation.Normalize();
				ImpulseVector = UKismetMathLibrary::MakeRotFromX(NewRotation).Vector();
				
			}
			
		   // UE_LOG(LogTemp,Log,TEXT("VECTOR  % f "),TouchPadDir.X);		
			Player->GetCharacterMovement()->AddImpulse(ImpulseVector  * m_SetVelocity,true);
	}
}

void USPRollDisplacemenTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context,const EAbleAbilityTaskResult result) const
{
	Super::OnTaskEnd(Context, result);
	OnTaskEndBP(Context.Get(), result);
}

void USPRollDisplacemenTask::OnTaskEndBP_Implementation(const UAbleAbilityContext* Context, const EAbleAbilityTaskResult result) const
{
	
}

bool USPRollDisplacemenTask::IsSingleFrame() const
{ 
	return IsSingleFrameBP();
}

bool USPRollDisplacemenTask::IsSingleFrameBP_Implementation() const
{
    return !m_IsDuration; 
}

UAbleAbilityTaskScratchPad* USPRollDisplacemenTask::CreateScratchPad(const TWeakObjectPtr<UAbleAbilityContext>& Context) const
{
	if (m_IsDuration)
	{
		if (UAbleAbilityUtilitySubsystem* Subsystem = Context->GetUtilitySubsystem())
		{
			static TSubclassOf<UAbleAbilityTaskScratchPad> ScratchPadClass = USPRollDisplacemenTaskScratchPad::StaticClass();
			return Subsystem->FindOrConstructTaskScratchPad(ScratchPadClass);
		}
		return NewObject<USPRollDisplacemenTaskScratchPad>(Context.Get());
	}
	return nullptr;
}

TStatId USPRollDisplacemenTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USPInvincibleTask, STATGROUP_USPAbility);
}

void USPRollDisplacemenTask::BindDynamicDelegates(UAbleAbility* Ability)
{
	Super::BindDynamicDelegates(Ability);
	ABL_BIND_DYNAMIC_PROPERTY(Ability, m_IsDuration, TEXT("IsDuration"));
	//ABL_BIND_DYNAMIC_PROPERTY(Ability, m_SetValue, TEXT("SetValue"));
	//ABL_BIND_DYNAMIC_PROPERTY(Ability, m_RestoreAtEnd, TEXT("RestoreAtEnd"));
	ABL_BIND_DYNAMIC_PROPERTY(Ability, m_SetVelocity, TEXT("SetVelocity"));
	ABL_BIND_DYNAMIC_PROPERTY(Ability, m_AddVelocity, TEXT("AddVelocity"));
}

#if WITH_EDITOR

FText USPRollDisplacemenTask::GetDescriptiveTaskName() const
{
	FText MyText = FText::FromString(TEXT("USPRollDisplacemenTask"));
	return MyText;
}

FText USPRollDisplacemenTask::GetRichTextTaskSummary() const
{
	FTextBuilder StringBuilder;
	StringBuilder.AppendLine(Super::GetRichTextTaskSummary());
	return StringBuilder.ToText();
}

EDataValidationResult USPRollDisplacemenTask::IsTaskDataValid(const UAbleAbility* AbilityContext,const FText& AssetName, TArray<FText>& ValidationErrors)
{
	EDataValidationResult result = EDataValidationResult::Valid;
	return  result;
}

#endif

EAbleAbilityTaskRealm USPRollDisplacemenTask::GetTaskRealmBP_Implementation() const { return ATR_Client; }

#undef LOCTEXT_NAMESPACE
// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SPGame/Skill/Task/SPInputControlTask.h"

#include "MoeFeatureSPLog.h"
#include "MoeGameLog.h"
#include "Game/SPGame/Character/SPGameCharacterBase.h"


#if WITH_EDITOR
FString GetEnumName(FString Enum, int EnumId)
{
	FString ReasonName = TEXT(""); 
	UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, *Enum, true);
	if (EnumPtr != nullptr)
	{
		ReasonName = EnumPtr->GetDisplayNameTextByIndex(EnumId).ToString();
	}
	else
	{
		ReasonName = FString::FromInt((int)EnumId);
	}
	return ReasonName;
}
FText USPInputControlTask::GetRichTextTaskSummary() const
{
	FTextBuilder StringBuilder;

	StringBuilder.AppendLine(Super::GetRichTextTaskSummary());

	FString InputControlTypeString = GetEnumName(TEXT("EInputControlType"), static_cast<int>(InputControlType));
	StringBuilder.AppendLine(FString::Printf(TEXT("\t- ControlType: %s"), *InputControlTypeString));

	return StringBuilder.ToText();
}
#endif



FString USPInputControlTask::GetModuleName_Implementation() const
{
	return TEXT("Feature.StarP.Script.System.Ability.Task.SPInputControlTask");
}

void USPInputControlTask::OnTaskStart(const TWeakObjectPtr<const UAbleAbilityContext>& Context) const
{
	MOE_SP_ABILITY_LOG(TEXT("USPInputControlTask::OnTaskStart"));
	Super::OnTaskStart(Context);
	OnTaskStartBP(Context.Get());
}

void USPInputControlTask::OnTaskStartBP_Implementation(const UAbleAbilityContext* Context) const
{
	//ASPGameCharacterBase* Player = Cast<ASPGameCharacterBase>(Context->GetOwner());
	//原本这个task的作用目标是Context:Owner,当这个task使用在啾灵上身able上时,啾灵上身able的context：owner是啾灵本身而不是玩家自己，导致这个task拿到的player是空,tick不会执行
	//现将这个task的作用目标改成target：self(默认),targetActor(啾灵上身able),这样可以解决bug
	//问题：原有的作用目标Context：owner和现在的target：self一定是同一个吗?如果是,这样改就不会有问题
	TArray<TWeakObjectPtr<AActor>> Targets;
	GetActorsForTask(Context, Targets);
	if (Targets.Num() <= 0) return;

	for (int i = 0; i < Targets.Num(); ++i)
	{
		if (!Targets[i].IsValid()) continue;

		ASPGameCharacterBase* Player = Cast<ASPGameCharacterBase>(Targets[i].Get());
		if (Player)
		{
			//修改为作用目标是targets之后,task内部就应该直接保存这些目标中是ASPGameCharacterBase类型的数组
			//这里可能会产生隐含问题,原本task的作用目标context:owner只会有一个,现在可能会有多个(取决与targets配置)
			m_TargetPlayers.Add(Player);
			USPGameCharInputComponent* InputComp = Player->FindComponentByClass<USPGameCharInputComponent>();
			if (InputComp != nullptr)
			{
				if (InputControlType != EInputControlType::None)
				{
					auto OverrideMove = [&, Context](ABaseMoeGameCharacter* Character, FVector Direction, float Scale)
					{
						//OnInput(Context, Direction, Scale);
					};
					InputComp->OverrideMoveFunc(true, OverrideMove);
				}
			}
		}
	}
}

void USPInputControlTask::OnTaskTick(const TWeakObjectPtr<const UAbleAbilityContext>& Context, float deltaTime) const
{
	Super::OnTaskTick(Context, deltaTime);
	OnTaskTickBP(Context.Get(), deltaTime);
}

void USPInputControlTask::OnTaskTickBP_Implementation(const UAbleAbilityContext* Context, float deltaTime) const
{
	if (m_TargetPlayers.Num() <= 0)
	{
		return;
	}
	//适配修改作用目标为多个后
	for (int i = 0; i < m_TargetPlayers.Num(); ++i)
	{
		if (m_TargetPlayers[i] != nullptr)
		{
			OnInput(Context, m_TargetPlayers[i], 1);
		}
	}
}

void USPInputControlTask::OnTaskEnd(const TWeakObjectPtr<const UAbleAbilityContext>& Context, const EAbleAbilityTaskResult result) const
{
	MOE_SP_ABILITY_LOG(TEXT("USPInputControlTask::OnTaskEnd"));
	//适配修改作用目标为多个后
	for (int i = 0; i < m_TargetPlayers.Num(); ++i) {
		if (m_TargetPlayers[i] != nullptr)
		{
			USPGameCharInputComponent* InputComp = m_TargetPlayers[i]->FindComponentByClass<USPGameCharInputComponent>();
			InputComp->OverrideMoveFunc(false, nullptr);
		}
	}
	
	Super::OnTaskEnd(Context, result);
	OnTaskEndBP(Context.Get(), result);
}

void USPInputControlTask::OnTaskEndBP_Implementation(const UAbleAbilityContext* Context, const EAbleAbilityTaskResult result) const
{
}

void USPInputControlTask::OnInput(const TWeakObjectPtr<const UAbleAbilityContext>& Context, ASPGameCharacterBase* InPlayer, float Scale) const
{
	if (Context.IsValid())
	{
		USPGameCharInputComponent* InputComp = InPlayer->FindComponentByClass<USPGameCharInputComponent>();
		const FVector moveInput = InputComp->GetCurrentJoystickInput();
		FVector Direction = moveInput.GetSafeNormal();
		if (InputControlType == EInputControlType::ListenInput)
        {
        	if (Direction.Size() > 0)
        	{
				FVector ApplyControlSpeedDirection = moveInput.GetSafeNormal2D();
        		ApplyControlSpeed(Context, InPlayer, ApplyControlSpeedDirection);

        		const UAbleAbility* Ability = Context.Get()->GetAbility();
        		USPAbilityComponent* AbleAbilityComp = Cast<USPAbilityComponent>(Context.Get()->GetSelfAbilityComponent());
        		if (Ability != nullptr && AbleAbilityComp != nullptr)
        		{
        			const int32 AbilityId = Context.Get()->GetAbilityId();
        			AbleAbilityComp->InterruptedAbility(AbilityId, "USPInputControlTask::OnInput");
        			MOE_SP_ABILITY_LOG(TEXT("USPInputControlTask::OnInput InterruptedAbility AbilityId %d"), AbilityId);
        		}
        	}
        }
        else if (InputControlType == EInputControlType::InputInterruptActionState)
        {
        	if (Direction.Size() > 0)
        	{
				FVector ApplyControlSpeedDirection = moveInput.GetSafeNormal2D();
        		ApplyControlSpeed(Context, InPlayer, ApplyControlSpeedDirection);

		        if (ActionState_Main != ECharActionState::None)
		        {
			        ASPGameCharacterBase* Player = Cast<ASPGameCharacterBase>(Context->GetOwner());
		        	if (Player)
		        	{
		        		Player->TryLeaveActionState_New(static_cast<uint8>(ActionState_Main), EStateIdOffsetType::Default,  false);
		        	}
		        }
        		else if (ActionState_SP != ESPActionState::SP_Begin)
        		{
        			ASPGameCharacterBase* Player = Cast<ASPGameCharacterBase>(Context->GetOwner());
        			if (Player)
        			{
        				Player->TryLeaveActionState_New(static_cast<uint8>(ActionState_SP), EStateIdOffsetType::SP,  false);
        			}
        		}
        	}
        }
	}
}

void USPInputControlTask::ApplyControlSpeed(const TWeakObjectPtr<const UAbleAbilityContext>& Context, ASPGameCharacterBase* InPlayer,const FVector& InDirection) const
{
	MOE_SP_ABILITY_LOG(TEXT("USPInputControlTask::OnInput get"));

	if (bUseControlSpeed)
	{
		//ASPGameCharacterBase* Player = Cast<ASPGameCharacterBase>(Context->GetOwner());
		if (InPlayer)
		{
			UCharacterMovementComponent* CharMoveComp = InPlayer->GetCharacterMovement();
			if (CharMoveComp != nullptr)
			{
				//这里保持结构，把下边这段挪到外边去,同时修改传参为[传这个函数处理的Player]和[要修改速度的向量]
				/*USPGameCharInputComponent* InputComp = InPlayer->FindComponentByClass<USPGameCharInputComponent>();
				if (InputComp != nullptr)
				{*/
					/*const FVector moveInput = InputComp->GetCurrentJoystickInput();
					FVector moveDir = moveInput.GetSafeNormal2D();*/
				FVector CurVelocity = CharMoveComp->Velocity;
				FVector NewVelocity = InDirection * CharMoveComp->GetMaxSpeed();
				CharMoveComp->Velocity = FVector(NewVelocity.X, NewVelocity.Y, CurVelocity.Z);
			}
		}
	}
}

bool USPInputControlTask::IsSingleFrameBP_Implementation() const { return false; }

EAbleAbilityTaskRealm USPInputControlTask::GetTaskRealmBP_Implementation() const { return EAbleAbilityTaskRealm::ATR_Client; }

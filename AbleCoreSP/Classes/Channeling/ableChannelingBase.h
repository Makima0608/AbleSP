// Copyright (c) Extra Life Studios, LLC. All rights reserved.

#pragma once

#include "ableAbilityTypes.h"
#include "ableAbilityContext.h"
#include "UObject/ObjectMacros.h"
#include "ableChannelingBase.generated.h"

#define LOCTEXT_NAMESPACE "AbleAbilityChanneling"

UCLASS(Abstract, EditInlineNew)
class ABLECORESP_API UAbleChannelingBase : public UObject
{
	GENERATED_BODY()
public:
	UAbleChannelingBase(const FObjectInitializer& ObjectInitializer);
	virtual ~UAbleChannelingBase();

	EAbleConditionResults GetConditionResult(UAbleAbilityContext& Context) const;

	const FString& GetDynamicPropertyIdentifier() const { return m_DynamicPropertyIdentifer; }

	/* Get Dynamic Delegate Name. */
	FName GetDynamicDelegateName(const FString& PropertyName) const;

	/* Bind any Dynamic Delegates. */
	virtual void BindDynamicDelegates(class UAbleAbility* Ability) {}

	/* If true, we'll ask the Server to cancel this Ability. */
	virtual bool RequiresServerNotificationOfFailure() const { return false; }

#if WITH_EDITOR
	bool FixUpObjectFlags();
#endif
protected:
	/* Override in your child class.*/
	virtual EAbleConditionResults CheckConditional(UAbleAbilityContext& Context) const { return EAbleConditionResults::ACR_Passed; }

	/* Negates the result of the condition (so false becomes true and vice versa). */
	UPROPERTY(EditAnywhere, Category = "Conditional", meta=(DisplayName="Negate"))
	bool m_Negate;

	/* The Identifier applied to any Dynamic Property methods for this task. This can be used to differentiate multiple tasks of the same type from each other within the same Ability. */
	UPROPERTY(EditInstanceOnly, Category = "Dynamic Properties", meta = (DisplayName = "Identifier"))
	FString m_DynamicPropertyIdentifer;
};

#undef LOCTEXT_NAMESPACE

// Copyright (c) Extra Life Studios, LLC. All rights reserved.

#pragma once

#include "Engine/Blueprint.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

#include "ableAbilityBlueprint.generated.h"

UCLASS(BlueprintType)
class ABLECORESP_API UAbleAbilityBlueprint : public UBlueprint
{
	GENERATED_UCLASS_BODY()

#if WITH_EDITOR

	/* Returns the UClass of our Ability Blueprints. */
	virtual UClass* GetBlueprintClass() const override;

	// UBlueprint interface
	virtual bool SupportedByDefaultBlueprintFactory() const override
	{
		return false;
	}
	// End of UBlueprint interface

	/** Returns the most base ability blueprint for a given blueprint (if it is inherited from another ability blueprint, returning null if only native / non-ability BP classes are it's parent) */
	static UAbleAbilityBlueprint* FindRootGameplayAbilityBlueprint(UAbleAbilityBlueprint* DerivedBlueprint);

	virtual bool AlwaysCompileOnLoad() const override;
#endif
};
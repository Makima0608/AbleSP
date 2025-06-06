// Copyright (c) Extra Life Studios, LLC. All rights reserved.

#pragma once

#include "Engine/EngineTypes.h"
#include "Targeting/ableTargetingBase.h"
#include "UObject/ObjectMacros.h"
#include "GenericTeamAgentInterface.h"

#include "ableCollisionFilters.generated.h"

struct FAbleQueryResult;
class UAbleAbilityContext;

#define LOCTEXT_NAMESPACE "AbleAbilityTask"

/* Base class for all Collision Filters. */
UCLASS(Abstract, BlueprintType)
class ABLECORESP_API UAbleCollisionFilter : public UObject
{
	GENERATED_BODY()
public:
	UAbleCollisionFilter(const FObjectInitializer& ObjectInitializer);
	virtual ~UAbleCollisionFilter();

	/* Perform our filter logic. */
	virtual void Filter(const TWeakObjectPtr<const UAbleAbilityContext>& Context, TArray<FAbleQueryResult>& InOutArray) const;

	/* Get our Dynamic Identifier. */
	const FString& GetDynamicPropertyIdentifier() const { return m_DynamicPropertyIdentifer; }

	/* Get Dynamic Delegate Name. */
	FName GetDynamicDelegateName(const FString& PropertyName) const;

	/* Bind any Dynamic Delegates */
	virtual void BindDynamicDelegates(class UAbleAbility* Ability) {};

#if WITH_EDITOR
	/* Data Validation Tests. */
    virtual EDataValidationResult IsTaskDataValid(const UAbleAbility* AbilityContext, const FText& AssetName, TArray<FText>& ValidationErrors) { return EDataValidationResult::Valid; }

	/* Fix our flags. */
	bool FixUpObjectFlags();
#endif
private:
	/* The Identifier applied to any Dynamic Property methods for this task. This can be used to differentiate multiple tasks of the same type from each other within the same Ability. */
	UPROPERTY(EditInstanceOnly, Category = "Dynamic Properties", meta = (DisplayName = "Identifier"))
	FString m_DynamicPropertyIdentifer;
};

UCLASS(EditInlineNew, meta = (DisplayName = "Filter Self", ShortToolTip = "Filters out the Self Actor."))
class ABLECORESP_API UAbleCollisionFilterSelf : public UAbleCollisionFilter
{
	GENERATED_BODY()
public:
	UAbleCollisionFilterSelf(const FObjectInitializer& ObjectInitializer);
	virtual ~UAbleCollisionFilterSelf();

	/* Perform our filter logic. */
	virtual void Filter(const TWeakObjectPtr<const UAbleAbilityContext>& Context, TArray<FAbleQueryResult>& InOutArray) const;

#if WITH_EDITOR
	/* Data Validation Tests. */
    virtual EDataValidationResult IsTaskDataValid(const UAbleAbility* AbilityContext, const FText& AssetName, TArray<FText>& ValidationErrors);
#endif
};

UCLASS(EditInlineNew, meta = (DisplayName = "Filter Owner", ShortToolTip = "Filters out the Owner Actor."))
class ABLECORESP_API UAbleCollisionFilterOwner : public UAbleCollisionFilter
{
	GENERATED_BODY()
public:
	UAbleCollisionFilterOwner(const FObjectInitializer& ObjectInitializer);
	virtual ~UAbleCollisionFilterOwner();

	/* Perform our filter logic. */
	virtual void Filter(const TWeakObjectPtr<const UAbleAbilityContext>& Context, TArray<FAbleQueryResult>& InOutArray) const;

#if WITH_EDITOR
	/* Data Validation Tests. */
    virtual EDataValidationResult IsTaskDataValid(const UAbleAbility* AbilityContext, const FText& AssetName, TArray<FText>& ValidationErrors);
#endif
};

UCLASS(EditInlineNew, meta = (DisplayName = "Filter Instigator", ShortToolTip = "Filters out the Instigator Actor."))
class ABLECORESP_API UAbleCollisionFilterInstigator : public UAbleCollisionFilter
{
	GENERATED_BODY()
public:
	UAbleCollisionFilterInstigator(const FObjectInitializer& ObjectInitializer);
	virtual ~UAbleCollisionFilterInstigator();

	/* Perform our filter logic. */
	virtual void Filter(const TWeakObjectPtr<const UAbleAbilityContext>& Context, TArray<FAbleQueryResult>& InOutArray) const;

#if WITH_EDITOR
	/* Data Validation Tests. */
    virtual EDataValidationResult IsTaskDataValid(const UAbleAbility* AbilityContext, const FText& AssetName, TArray<FText>& ValidationErrors);
#endif
};

UCLASS(EditInlineNew, meta = (DisplayName = "Filter By Class", ShortToolTip = "Filters out any instances of the provided class, can also be negated to keep only instances of the provided class."))
class ABLECORESP_API UAbleCollisionFilterByClass : public UAbleCollisionFilter
{
	GENERATED_BODY()
public:
	UAbleCollisionFilterByClass(const FObjectInitializer& ObjectInitializer);
	virtual ~UAbleCollisionFilterByClass();

	/* Perform our filter logic. */
	virtual void Filter(const TWeakObjectPtr<const UAbleAbilityContext>& Context, TArray<FAbleQueryResult>& InOutArray) const;

#if WITH_EDITOR
	/* Data Validation Tests. */
    virtual EDataValidationResult IsTaskDataValid(const UAbleAbility* AbilityContext, const FText& AssetName, TArray<FText>& ValidationErrors);
#endif
protected:
	/* The Class to filter. */
	UPROPERTY(EditInstanceOnly, Category = "Filter", meta = (DisplayName = "Class"))
	const UClass* m_Class;

	/* If true, the filter will keep only items that are of the provided class rather than filter them out. */
	UPROPERTY(EditInstanceOnly, Category = "Filter", meta = (DisplayName = "Negate"))
	bool m_Negate;
};

UENUM()
enum EAbleCollisionFilterSort
{
	AbleFitlerSort_Ascending = 0 UMETA(DisplayName="Ascending"),
	AbleFilterSort_Descending UMETA(DisplayName="Descending")
};

UCLASS(EditInlineNew, meta = (DisplayName = "Sort by Distance", ShortToolTip = "Sorts Targets from Nearest to Furthest (Ascending), or Furthest to Nearest (Descending)."))
class ABLECORESP_API UAbleCollisionFilterSortByDistance : public UAbleCollisionFilter
{
	GENERATED_BODY()
public:
	UAbleCollisionFilterSortByDistance(const FObjectInitializer& ObjectInitializer);
	virtual ~UAbleCollisionFilterSortByDistance();

	/* Perform our filter logic. */
	virtual void Filter(const TWeakObjectPtr<const UAbleAbilityContext>& Context, TArray<FAbleQueryResult>& InOutArray) const;

	/* Bind any Dynamic Delegates. */
	virtual void BindDynamicDelegates(class UAbleAbility* Ability) override;
#if WITH_EDITOR
	/* Data Validation Tests. */
    virtual EDataValidationResult IsTaskDataValid(const UAbleAbility* AbilityContext, const FText& AssetName, TArray<FText>& ValidationErrors);
#endif
protected:
	/* Ascending will sort from closest to furthest, descending will be the opposite.*/
	UPROPERTY(EditInstanceOnly, Category = "Filter", meta = (DisplayName = "Sort Direction"))
	TEnumAsByte<EAbleCollisionFilterSort> m_SortDirection;

	/* The Location to use as the source of our distance comparison. */
	UPROPERTY(EditInstanceOnly, Category = "Filter", meta = (DisplayName = "Source Location", AbleBindableProperty))
	FAbleAbilityTargetTypeLocation m_Location;

	UPROPERTY()
	FGetAbleTargetLocation m_LocationDelegate;

	/* If true, we will only use the 2D (XY Plane) distance rather than the 3D distance. */
	UPROPERTY(EditInstanceOnly, Category = "Filter", meta = (DisplayName = "Use 2D (XY) Distance)"))
	bool m_Use2DDistance;
};

UCLASS(EditInlineNew, meta = (DisplayName = "Max Limit", ShortToolTip = "Keeps Target results limited to Max Entities."))
class ABLECORESP_API UAbleCollisionFilterMaxResults : public UAbleCollisionFilter
{
	GENERATED_BODY()
public:
	UAbleCollisionFilterMaxResults(const FObjectInitializer& ObjectInitializer);
	virtual ~UAbleCollisionFilterMaxResults();

	/* Perform our filter logic. */
	virtual void Filter(const TWeakObjectPtr<const UAbleAbilityContext>& Context, TArray<FAbleQueryResult>& InOutArray) const;

#if WITH_EDITOR
	/* Data Validation Tests. */
    virtual EDataValidationResult IsTaskDataValid(const UAbleAbility* AbilityContext, const FText& AssetName, TArray<FText>& ValidationErrors);
#endif
protected:
	/* The Maximum number of results you would like. Anything above this is removed.*/
	UPROPERTY(EditInstanceOnly, Category = "Filter", meta = (DisplayName = "Max Entities"))
	int32 m_MaxEntities;
};

UCLASS(EditInlineNew, meta = (DisplayName = "Custom", ShortToolTip = "Calls the Ability's IsValidForActor Blueprint Event. If the event returns true, the actor is kept. If false, it is discarded."))
class ABLECORESP_API UAbleCollisionFilterCustom : public UAbleCollisionFilter
{
	GENERATED_BODY()
public:
	UAbleCollisionFilterCustom(const FObjectInitializer& ObjectInitializer);
	virtual ~UAbleCollisionFilterCustom();

	/* Perform our filter logic. */
	virtual void Filter(const TWeakObjectPtr<const UAbleAbilityContext>& Context, TArray<FAbleQueryResult>& InOutArray) const;

#if WITH_EDITOR
	/* Data Validation Tests. */
    virtual EDataValidationResult IsTaskDataValid(const UAbleAbility* AbilityContext, const FText& AssetName, TArray<FText>& ValidationErrors);
#endif
protected:
	// Optional Name identifier for this event in case you are using IsValidForActor multiple times in the Ability.
	UPROPERTY(EditInstanceOnly, Category = "Filter", meta = (DisplayName = "Event Name"))
	FName m_EventName;

	// If true, the event is run across multiple actors on various cores. This can help speed things up if the potential actor list is large, or the BP logic is complex.
	UPROPERTY(EditInstanceOnly, Category = "Optimize", meta = (DisplayName = "Use Async"))
	bool m_UseAsync;
};

UCLASS(EditInlineNew, meta = (DisplayName = "Filter Attitude", ShortToolTip = "Ignore Actors of particular attitudes, must implement IGenericTeamAgentInterface."))
class UAbleCollisionFilterTeamAttitude : public UAbleCollisionFilter
{
	GENERATED_BODY()
public:
	UAbleCollisionFilterTeamAttitude(const FObjectInitializer& ObjectInitializer);
	virtual ~UAbleCollisionFilterTeamAttitude();

	virtual void Filter(const TWeakObjectPtr<const UAbleAbilityContext>& Context, TArray<FAbleQueryResult>& InOutArray) const;

#if WITH_EDITOR
	virtual EDataValidationResult IsAbilityDataValid(const UAbleAbility* AbilityContext, TArray<FText>& ValidationErrors);
#endif
protected:
	UPROPERTY(EditInstanceOnly, Category = "Filter", meta = (DisplayName = "Ignore Attitude", Bitmask, BitmaskEnum = ETeamAttitude))
	int32 m_IgnoreAttitude;
};

UCLASS(EditInlineNew, meta = (DisplayName = "Line Of Sight", ShortToolTip = "Casts a ray between the Target and the Source Location(Actor, Location, etc). If a blocking hit is found between the two, the target is discarded."))
class ABLECORESP_API UAbleCollisionFilterLineOfSight : public UAbleCollisionFilter
{
	GENERATED_BODY()
public:
	UAbleCollisionFilterLineOfSight(const FObjectInitializer& ObjectInitializer);
	virtual ~UAbleCollisionFilterLineOfSight();

	/* Perform our filter logic. */
	virtual void Filter(const TWeakObjectPtr<const UAbleAbilityContext>& Context, TArray<FAbleQueryResult>& InOutArray) const;

	/* Bind any Dynamic Delegates. */
	virtual void BindDynamicDelegates(class UAbleAbility* Ability) override;
#if WITH_EDITOR
	/* Data Validation Tests. */
	virtual EDataValidationResult IsTaskDataValid(const UAbleAbility* AbilityContext, const FText& AssetName, TArray<FText>& ValidationErrors);
#endif
protected:
	// The Location to use as our source for our raycast.
	UPROPERTY(EditInstanceOnly, Category = "Filter", meta = (DisplayName = "Source Location", AbleBindableProperty))
	FAbleAbilityTargetTypeLocation m_Location;

	UPROPERTY()
	FGetAbleTargetLocation m_LocationDelegate;

	UPROPERTY(EditInstanceOnly, Category = "Filter", meta = (DisplayName = "Collision Channel Present"))
	TEnumAsByte<EAbleChannelPresent> m_ChannelPresent;
	
	// The Collision Channels to run the Raycast against.
	UPROPERTY(EditInstanceOnly, Category = "Filter", meta = (DisplayName = "Collision Channels"))
	TArray<TEnumAsByte<ESPAbleTraceType>> m_CollisionChannels;
};

UCLASS(EditInlineNew, meta = (DisplayName = "Unique Actors", ShortToolTip = "Reduce Collision results to only the unique results based on Actors hit rather than components hit."))
class UAbleCollisionFilterUniqueActors : public UAbleCollisionFilter
{
	GENERATED_BODY()
public:
	UAbleCollisionFilterUniqueActors(const FObjectInitializer& ObjectInitializer);
	virtual ~UAbleCollisionFilterUniqueActors();

	virtual void Filter(const TWeakObjectPtr<const UAbleAbilityContext>& Context, TArray<FAbleQueryResult>& InOutArray) const;

#if WITH_EDITOR
	virtual EDataValidationResult IsAbilityDataValid(const UAbleAbility* AbilityContext, TArray<FText>& ValidationErrors);
#endif
};

#undef LOCTEXT_NAMESPACE
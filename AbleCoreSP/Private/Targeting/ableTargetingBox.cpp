// Copyright (c) Extra Life Studios, LLC. All rights reserved.

#include "Targeting/ableTargetingBox.h"

#include "ableAbility.h"
#include "ableAbilityDebug.h"
#include "ableSettings.h"
#include "Engine/World.h"

UAbleTargetingBox::UAbleTargetingBox(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	m_HalfExtents(50.0f, 50.0f, 50.0f)
{

}

UAbleTargetingBox::~UAbleTargetingBox()
{

}

void UAbleTargetingBox::FindTargets(UAbleAbilityContext& Context) const
{
	FAbleAbilityTargetTypeLocation Location = ABL_GET_DYNAMIC_PROPERTY_VALUE_RAW(&Context, m_Location);

	AActor* SourceActor = Location.GetSourceActor(Context);
    if (SourceActor == nullptr)
	{
        return;
	}

	UWorld* World = SourceActor->GetWorld();
	FTransform QueryTransform;

	FVector HalfExtents = ABL_GET_DYNAMIC_PROPERTY_VALUE_RAW(&Context, m_HalfExtents);

	FCollisionObjectQueryParams ObjectQuery;
	GetCollisionObjectParams(&Context, ObjectQuery);

	if (IsUsingAsync() && USPAbleSettings::IsAsyncEnabled())
	{
		// Check if we have a valid Async handle already. 
		if (!Context.HasValidAsyncHandle())
		{
			FCollisionShape BoxShape = FCollisionShape::MakeBox(HalfExtents);

			Location.GetTransform(Context, QueryTransform);

			// Push our query out by our half extents so we aren't centered in the box.
			FQuat Rotation = QueryTransform.GetRotation();

			FVector HalfExtentsOffset = Rotation.GetForwardVector() * HalfExtents.X;

			QueryTransform *= FTransform(HalfExtentsOffset);

			FTraceHandle AsyncHandle = World->AsyncOverlapByObjectType(QueryTransform.GetLocation(), QueryTransform.GetRotation(), ObjectQuery, BoxShape);
			Context.SetAsyncHandle(AsyncHandle);
		}
		else // Poll and see if our query is done, if so - process it.
		{
			FOverlapDatum Datum;
			if (World->QueryOverlapData(Context.GetAsyncHandle(), Datum))
			{
				ProcessResults(Context, Datum.OutOverlaps);

				FTraceHandle Empty;
				Context.SetAsyncHandle(Empty); // Reset our handle.
			}

			return;
		}
	}
	else // Normal Sync Query
	{
		FCollisionShape BoxShape = FCollisionShape::MakeBox(HalfExtents);

		Location.GetTransform(Context, QueryTransform);

		// Push our query out by our half extents so we aren't centered in the box.
		FQuat Rotation = QueryTransform.GetRotation();

		FVector HalfExtentsOffset = Rotation.GetForwardVector() * HalfExtents.X;

		QueryTransform *= FTransform(HalfExtentsOffset);

		TArray<FOverlapResult> Results;
		if (World->OverlapMultiByObjectType(Results, QueryTransform.GetLocation(), QueryTransform.GetRotation(), ObjectQuery, BoxShape))
		{
			ProcessResults(Context, Results);
		}
	}

#if !UE_BUILD_SHIPPING
	if (FAbleAbilityDebug::ShouldDrawQueries())
	{
		// Nope, go ahead and fire off our Async query.
		FVector AlignedBox = GetAlignedBox(Context, QueryTransform);

		FAbleAbilityDebug::DrawBoxQuery(World, QueryTransform, AlignedBox);
	}
#endif // UE_BUILD_SHIPPING
}

void UAbleTargetingBox::BindDynamicDelegates(UAbleAbility* Ability)
{
	ABL_BIND_DYNAMIC_PROPERTY(Ability, m_HalfExtents, "Half Extents");

	Super::BindDynamicDelegates(Ability);
}

float UAbleTargetingBox::CalculateRange() const
{
	FVector RotatedBox;
	FQuat Rotation = FQuat(m_Location.GetRotation());

	RotatedBox = Rotation.GetForwardVector() + m_HalfExtents.X;
	RotatedBox += Rotation.GetRightVector() + m_HalfExtents.Y;
	RotatedBox += Rotation.GetUpVector() + m_HalfExtents.Z;

	if (m_CalculateAs2DRange)
	{
		return m_Location.GetOffset().Size2D() + RotatedBox.Size2D();
	}
	
	return m_Location.GetOffset().Size() + RotatedBox.Size();
}

FVector UAbleTargetingBox::GetAlignedBox(const UAbleAbilityContext& Context, FTransform& OutQueryTransform) const
{
	FVector AlignedBox;

	FAbleAbilityTargetTypeLocation Location = ABL_GET_DYNAMIC_PROPERTY_VALUE_RAW(&Context, m_Location);

	Location.GetTransform(Context, OutQueryTransform);

	const FQuat QueryRotation = OutQueryTransform.GetRotation();

	FVector HalfExtents = ABL_GET_DYNAMIC_PROPERTY_VALUE_RAW(&Context, m_HalfExtents);

	// Somewhere down the pipeline, the Box rotation is ignored, so go ahead and take care of it here.
	AlignedBox = QueryRotation.GetForwardVector() * HalfExtents.X;
	AlignedBox += QueryRotation.GetRightVector() * HalfExtents.Y;
	AlignedBox += QueryRotation.GetUpVector() * HalfExtents.Z;

	// Move out by our Half Extents.
	FVector HalfExtentsOffset = QueryRotation.GetForwardVector() * HalfExtents.X;
	OutQueryTransform *= FTransform(HalfExtentsOffset);

	return AlignedBox;
}

void UAbleTargetingBox::ProcessResults(UAbleAbilityContext& Context, const TArray<struct FOverlapResult>& Results) const
{
	TArray<TWeakObjectPtr<AActor>>& TargetActors = Context.GetMutableTargetActors();

	for (const FOverlapResult& Result : Results)
	{
		if (AActor* ResultActor = Result.GetActor())
		{
			TargetActors.Add(ResultActor);
		}
	}

	// Call our filters.
	FilterTargets(Context);
}

#if WITH_EDITOR
void UAbleTargetingBox::OnAbilityEditorTick(const UAbleAbilityContext& Context, float DeltaTime) const
{
	// Draw a Preview.
	AActor* SourceActor = m_Location.GetSourceActor(Context);
	if (SourceActor != nullptr)
	{
		FTransform QueryTransform;
		m_Location.GetTransform(Context, QueryTransform);

		FVector AlignedBox = QueryTransform.GetRotation().GetForwardVector() * m_HalfExtents.X;
		AlignedBox += QueryTransform.GetRotation().GetRightVector() * m_HalfExtents.Y;
		AlignedBox += QueryTransform.GetRotation().GetUpVector() * m_HalfExtents.Z;
		FAbleAbilityDebug::DrawBoxQuery(Context.GetWorld(), QueryTransform, m_HalfExtents);
	}
}
#endif
#pragma once
#include "Village/House/House.h"
#include "FKnownHouse_MartensTuur.generated.h"

USTRUCT()
struct FKnownHouse_MartensTuur
{
	GENERATED_BODY()
	
	FHouseBounds Bounds;
	// TODO: may need to re-check a while after we take some items, in case a new item spawned
	bool HasChecked{false};
	
	[[nodiscard]]
	bool operator==(const FKnownHouse_MartensTuur& Other) const
	{
		// HasChecked has no bearing on whether one house is equal to another
		return Bounds.Extent == Other.Bounds.Extent
			&& Bounds.Origin == Other.Bounds.Origin;
	}
};

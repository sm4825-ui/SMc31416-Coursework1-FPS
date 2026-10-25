#include "EquippableToolDefinition.h"
#include "EquippableToolBase.h"
#include "UObject/UObjectGlobals.h"
#include "UObject/Package.h"

UItemDefinition* UEquippableToolDefinition::CreateItemCopy(UObject* Outer) const
{
	if (!Outer)
	{
		Outer = GetTransientPackage();
	}

	return DuplicateObject<UEquippableToolDefinition>(this, Outer);
}

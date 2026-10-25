#include "ItemDefinition.h"
#include "UObject/Package.h"

UItemDefinition* UItemDefinition::CreateItemCopy(UObject* Outer) const
{
    if (!Outer)
    {
        Outer = GetTransientPackage();
    }

    UItemDefinition* ItemCopy = NewObject<UItemDefinition>(Outer);

    ItemCopy->ID = ID;
    ItemCopy->ItemType = ItemType;
    ItemCopy->ItemText = ItemText;
    ItemCopy->WorldMesh = WorldMesh;

    return ItemCopy;
}

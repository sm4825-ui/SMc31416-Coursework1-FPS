#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/StaticMesh.h"
#include "Data/ItemData.h"
#include "ItemDefinition.generated.h"

UCLASS(BlueprintType, Blueprintable)
class ADVENTUREGAME_API UItemDefinition : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
    FName ID;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
    EItemType ItemType;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
    FItemText ItemText;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
    TSoftObjectPtr<UStaticMesh> WorldMesh;

    virtual UItemDefinition* CreateItemCopy(UObject* Outer) const;
};

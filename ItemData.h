#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UObject/SoftObjectPtr.h"
#include "ItemData.generated.h"

class UItemDefinition;

UENUM(BlueprintType)
enum class EItemType : uint8
{
    Tool UMETA(DisplayName = "Tool"),
    Consumable UMETA(DisplayName = "Consumable")
};

USTRUCT(BlueprintType)
struct FItemText
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText Name;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText Description;
};

USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
    FName ID;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
    EItemType ItemType;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
    FItemText ItemText;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
    TSoftObjectPtr<UItemDefinition> ItemBase;
};
#pragma once

#include "CoreMinimal.h"
#include "ItemDefinition.h"
#include "EquippableToolDefinition.generated.h"

class AEquippableToolBase;

UCLASS(BlueprintType, Blueprintable)
class ADVENTUREGAME_API UEquippableToolDefinition : public UItemDefinition
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tool")
	TSubclassOf<AEquippableToolBase> ToolAsset;

	virtual UItemDefinition* CreateItemCopy(UObject* Outer) const override;
};

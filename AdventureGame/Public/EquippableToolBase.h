#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"
#include "EquippableToolBase.generated.h"

class UAnimBlueprint;
class AAdventureCharacter;
class UInputMappingContext;
class UInputAction;
class UEnhancedInputComponent;

UCLASS(BlueprintType, Blueprintable)
class ADVENTUREGAME_API AEquippableToolBase : public AActor
{
	GENERATED_BODY()

public:
	AEquippableToolBase();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tool")
	TObjectPtr<USkeletalMeshComponent> ToolMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimBlueprint> FirstPersonToolAnim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimBlueprint> ThirdPersonToolAnim;

	UPROPERTY(BlueprintReadWrite, Category = "Tool")
	AAdventureCharacter* OwningCharacter;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> ToolMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> UseAction;

	UFUNCTION()
	virtual void Use();

	UFUNCTION()
	virtual void BindInputAction(const UInputAction* ActionToBind);
};

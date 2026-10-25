#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "AdventureCharacter.generated.h"

class UInputAction;
class UAnimBlueprint;
class UInventoryComponent;
class UEquippableToolDefinition;
class AEquippableToolBase;
class UItemDefinition;

UCLASS()
class ADVENTUREGAME_API AAdventureCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AAdventureCharacter();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputMappingContext> FirstPersonContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> JumpAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> LookAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> UseAction;

public:
    virtual void Tick(float DeltaTime) override;

    virtual void SetupPlayerInputComponent(
        UInputComponent* PlayerInputComponent) override;

    UFUNCTION()
    void Move(const FInputActionValue& Value);

    UFUNCTION()
    void Look(const FInputActionValue& Value);

    UFUNCTION()
    FVector GetCameraTargetLocation();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    TObjectPtr<UCameraComponent> FirstPersonCameraComponent;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
    FVector FirstPersonCameraOffset = FVector(2.8f, 5.9f, 0.0f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
    float FirstPersonFieldOfView = 70.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
    TObjectPtr<USkeletalMeshComponent> FirstPersonMeshComponent;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    TObjectPtr<UAnimBlueprint> FirstPersonDefaultAnim;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    TObjectPtr<UInventoryComponent> InventoryComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tools")
    TObjectPtr<AEquippableToolBase> EquippedTool;

    UFUNCTION(BlueprintCallable, Category = "Tools")
    bool IsToolAlreadyOwned(
        UEquippableToolDefinition* ToolDefinition);

    UFUNCTION(BlueprintCallable, Category = "Tools")
    void AttachTool(UEquippableToolDefinition* ToolDefinition);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void GiveItem(UItemDefinition* ItemDefinition);

    void AddToolInputMapping(AEquippableToolBase* Tool);
};

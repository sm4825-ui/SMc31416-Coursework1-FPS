
#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "AdventureCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class UAnimBlueprint;

UCLASS()
class ADVENTUREGAME_API AAdventureCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AAdventureCharacter();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    TObjectPtr<UInputMappingContext> FirstPersonContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    TObjectPtr<UInputAction> MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    TObjectPtr<UInputAction> JumpAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    TObjectPtr<UInputAction> LookAction;

public:
    virtual void Tick(float DeltaTime) override;

    virtual void SetupPlayerInputComponent(
        class UInputComponent* PlayerInputComponent) override;

    UFUNCTION()
    void Move(const FInputActionValue& Value);

    UFUNCTION()
    void Look(const FInputActionValue& Value);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    TObjectPtr<UCameraComponent> FirstPersonCameraComponent;

    UPROPERTY(EditAnywhere, Category = Camera)
    FVector FirstPersonCameraOffset = FVector(2.8f, 5.9f, 0.0f);

    UPROPERTY(EditAnywhere, Category = Camera)
    float FirstPersonFieldOfView = 70.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
    TObjectPtr<USkeletalMeshComponent> FirstPersonMeshComponent;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
    TObjectPtr<UAnimBlueprint> FirstPersonDefaultAnim;
};

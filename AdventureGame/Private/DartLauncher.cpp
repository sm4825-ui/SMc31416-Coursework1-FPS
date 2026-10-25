#include "DartLauncher.h"

#include "AdventureCharacter.h"
#include "Engine/World.h"
#include "FirstPersonProjectile.h"
#include "EnhancedInputComponent.h"
#include "Engine/Engine.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/PlayerController.h"

void ADartLauncher::Use()
{
    UE_LOG(LogTemp, Warning, TEXT("DartLauncher::Use fired."));

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            5.0f,
            FColor::Yellow,
            TEXT("Using the dart launcher!"));
    }

    if (!OwningCharacter || !ToolMeshComponent || !ProjectileClass)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("Dart launcher is missing its owner, mesh, or projectile class."));
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    const FVector SocketLocation =
        ToolMeshComponent->GetSocketLocation(FName(TEXT("Muzzle")));
    const FVector TargetPosition =
        OwningCharacter->GetCameraTargetLocation();
    const FRotator SpawnRotation =
        UKismetMathLibrary::FindLookAtRotation(SocketLocation, TargetPosition);
    const FVector SpawnLocation =
        SocketLocation + SpawnRotation.Vector() * 10.0f;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride =
        ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

    World->SpawnActor<AFirstPersonProjectile>(
        ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);
}

void ADartLauncher::BindInputAction(
    const UInputAction* InputToBind)
{
    if (!OwningCharacter)
    {
        UE_LOG(LogTemp, Error,
            TEXT("DartLauncher input failed: OwningCharacter is null."));
        return;
    }

    if (!InputToBind)
    {
        UE_LOG(LogTemp, Error,
            TEXT("DartLauncher input failed: InputAction is null."));
        return;
    }

    APlayerController* PlayerController =
        Cast<APlayerController>(OwningCharacter->GetController());

    if (!PlayerController)
    {
        UE_LOG(LogTemp, Error,
            TEXT("DartLauncher input failed: PlayerController is null."));
        return;
    }

    UEnhancedInputComponent* EnhancedInputComponent =
        Cast<UEnhancedInputComponent>(PlayerController->InputComponent);

    if (!EnhancedInputComponent)
    {
        UE_LOG(LogTemp, Error,
            TEXT("DartLauncher input failed: EnhancedInputComponent is null."));
        return;
    }

    EnhancedInputComponent->BindAction(
        InputToBind,
        ETriggerEvent::Triggered,
        this,
        &ADartLauncher::Use);

    UE_LOG(LogTemp, Warning,
        TEXT("DartLauncher bound action: %s"),
        *InputToBind->GetName());
}

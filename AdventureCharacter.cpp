
#include "AdventureCharacter.h"

#include "Animation/AnimBlueprint.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"

AAdventureCharacter::AAdventureCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    FirstPersonMeshComponent =
        CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));

    FirstPersonMeshComponent->SetupAttachment(GetMesh());
    FirstPersonMeshComponent->SetOnlyOwnerSee(true);
    FirstPersonMeshComponent->SetCollisionProfileName(TEXT("NoCollision"));
    FirstPersonMeshComponent->bCastDynamicShadow = false;
    FirstPersonMeshComponent->CastShadow = false;

    FirstPersonCameraComponent =
        CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));

    FirstPersonCameraComponent->SetupAttachment(
        FirstPersonMeshComponent, TEXT("head"));

    FirstPersonCameraComponent->SetRelativeLocationAndRotation(
        FirstPersonCameraOffset,
        FRotator(0.0f, 90.0f, -90.0f));

    FirstPersonCameraComponent->bUsePawnControlRotation = true;
    FirstPersonCameraComponent->SetFieldOfView(FirstPersonFieldOfView);

    GetMesh()->SetOwnerNoSee(true);
}

void AAdventureCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (APlayerController* PlayerController =
        Cast<APlayerController>(GetController()))
    {
        if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
        {
            if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
                LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
            {
                if (FirstPersonContext)
                {
                    Subsystem->AddMappingContext(FirstPersonContext, 0);
                }
            }
        }
    }

    if (FirstPersonDefaultAnim)
    {
        FirstPersonMeshComponent->SetAnimInstanceClass(
            FirstPersonDefaultAnim->GeneratedClass);

        GetMesh()->SetAnimInstanceClass(
            FirstPersonDefaultAnim->GeneratedClass);
    }
}

void AAdventureCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AAdventureCharacter::SetupPlayerInputComponent(
    UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    UEnhancedInputComponent* EnhancedInputComponent =
        Cast<UEnhancedInputComponent>(PlayerInputComponent);

    if (EnhancedInputComponent)
    {
        EnhancedInputComponent->BindAction(
            MoveAction, ETriggerEvent::Triggered,
            this, &AAdventureCharacter::Move);

        EnhancedInputComponent->BindAction(
            JumpAction, ETriggerEvent::Started,
            this, &ACharacter::Jump);

        EnhancedInputComponent->BindAction(
            JumpAction, ETriggerEvent::Completed,
            this, &ACharacter::StopJumping);

        EnhancedInputComponent->BindAction(
            LookAction, ETriggerEvent::Triggered,
            this, &AAdventureCharacter::Look);
    }
}

void AAdventureCharacter::Move(const FInputActionValue& Value)
{
    const FVector2D MovementValue = Value.Get<FVector2D>();

    if (Controller)
    {
        AddMovementInput(GetActorRightVector(), MovementValue.X);
        AddMovementInput(GetActorForwardVector(), MovementValue.Y);
    }
}

void AAdventureCharacter::Look(const FInputActionValue& Value)
{
    const FVector2D LookAxisValue = Value.Get<FVector2D>();

    if (Controller)
    {
        AddControllerYawInput(LookAxisValue.X);
        AddControllerPitchInput(LookAxisValue.Y);
    }
}

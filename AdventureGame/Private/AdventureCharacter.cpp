#include "AdventureCharacter.h"

#include "Animation/AnimBlueprint.h"
#include "Animation/AnimInstance.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "InventoryComponent.h"
#include "EquippableToolDefinition.h"
#include "EquippableToolBase.h"
#include "ItemDefinition.h"
#include "InputMappingContext.h"
#include "Components/SkeletalMeshComponent.h"
#include "UObject/UnrealType.h"

AAdventureCharacter::AAdventureCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    FirstPersonMeshComponent =
        CreateDefaultSubobject<USkeletalMeshComponent>(
            TEXT("FirstPersonMesh"));

    FirstPersonMeshComponent->SetupAttachment(GetMesh());
    FirstPersonMeshComponent->SetOnlyOwnerSee(true);
    FirstPersonMeshComponent->SetCollisionProfileName(
        TEXT("NoCollision"));
    FirstPersonMeshComponent->bCastDynamicShadow = false;
    FirstPersonMeshComponent->CastShadow = false;

    FirstPersonCameraComponent =
        CreateDefaultSubobject<UCameraComponent>(
            TEXT("FirstPersonCamera"));

    FirstPersonCameraComponent->SetupAttachment(
        FirstPersonMeshComponent, TEXT("head"));

    FirstPersonCameraComponent->SetRelativeLocationAndRotation(
        FirstPersonCameraOffset,
        FRotator(0.0f, 90.0f, -90.0f));

    FirstPersonCameraComponent->bUsePawnControlRotation = true;
    FirstPersonCameraComponent->SetFieldOfView(
        FirstPersonFieldOfView);

    GetMesh()->SetOwnerNoSee(true);

    InventoryComponent =
        CreateDefaultSubobject<UInventoryComponent>(
            TEXT("InventoryComponent"));

    EquippedTool = nullptr;
}

void AAdventureCharacter::BeginPlay()
{
    Super::BeginPlay();

    APlayerController* PlayerController =
        Cast<APlayerController>(GetController());

    if (PlayerController)
    {
        ULocalPlayer* LocalPlayer =
            PlayerController->GetLocalPlayer();

        if (LocalPlayer)
        {
            UEnhancedInputLocalPlayerSubsystem* Subsystem =
                LocalPlayer->GetSubsystem<
                UEnhancedInputLocalPlayerSubsystem>();

            if (Subsystem && FirstPersonContext)
            {
                Subsystem->AddMappingContext(
                    FirstPersonContext, 0);
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

    UEnhancedInputComponent* EnhancedInput =
        Cast<UEnhancedInputComponent>(PlayerInputComponent);

    if (!EnhancedInput)
    {
        UE_LOG(LogTemp, Error,
            TEXT("Character input setup failed: not EnhancedInputComponent."));
        return;
    }

    if (MoveAction)
    {
        EnhancedInput->BindAction(
            MoveAction,
            ETriggerEvent::Triggered,
            this,
            &AAdventureCharacter::Move);
    }

    if (JumpAction)
    {
        EnhancedInput->BindAction(
            JumpAction,
            ETriggerEvent::Started,
            this,
            &ACharacter::Jump);

        EnhancedInput->BindAction(
            JumpAction,
            ETriggerEvent::Completed,
            this,
            &ACharacter::StopJumping);
    }

    if (LookAction)
    {
        EnhancedInput->BindAction(
            LookAction,
            ETriggerEvent::Triggered,
            this,
            &AAdventureCharacter::Look);
    }

    if (EquippedTool)
    {
        const UInputAction* ActionToBind = UseAction
            ? UseAction.Get()
            : EquippedTool->UseAction.Get();

        EquippedTool->BindInputAction(ActionToBind);
    }
}

void AAdventureCharacter::Move(
    const FInputActionValue& Value)
{
    const FVector2D MovementValue =
        Value.Get<FVector2D>();

    if (Controller)
    {
        AddMovementInput(
            GetActorRightVector(),
            MovementValue.X);

        AddMovementInput(
            GetActorForwardVector(),
            MovementValue.Y);
    }
}

void AAdventureCharacter::Look(
    const FInputActionValue& Value)
{
    const FVector2D LookAxisValue =
        Value.Get<FVector2D>();

    if (Controller)
    {
        AddControllerYawInput(LookAxisValue.X);
        AddControllerPitchInput(LookAxisValue.Y);
    }
}

FVector AAdventureCharacter::GetCameraTargetLocation()
{
	if (!FirstPersonCameraComponent)
	{
		return GetActorLocation() + GetActorForwardVector() * 10000.0f;
	}

	const FVector TraceStart = FirstPersonCameraComponent->GetComponentLocation();
    const FVector TraceEnd = TraceStart +
        FirstPersonCameraComponent->GetForwardVector() * 10000.0f;

	if (UWorld* World = GetWorld())
    {
		FHitResult Hit;
		FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(CameraTargetTrace), false, this);
		if (World->LineTraceSingleByChannel(
			Hit, TraceStart, TraceEnd, ECC_Visibility, TraceParams))
		{
			return Hit.ImpactPoint;
		}
    }

    return TraceEnd;
}

bool AAdventureCharacter::IsToolAlreadyOwned(
    UEquippableToolDefinition* ToolDefinition)
{
    if (!ToolDefinition || !InventoryComponent)
    {
        return false;
    }

    for (const TObjectPtr<UEquippableToolDefinition>& Item :
        InventoryComponent->ToolInventory)
    {
        if (Item && Item->ID == ToolDefinition->ID)
        {
            return true;
        }
    }

    return false;
}

void AAdventureCharacter::AttachTool(
    UEquippableToolDefinition* ToolDefinition)
{
    if (!ToolDefinition || !InventoryComponent)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("AttachTool failed: missing definition or inventory."));
        return;
    }

    if (IsToolAlreadyOwned(ToolDefinition))
    {
        return;
    }

    if (!ToolDefinition->ToolAsset || !GetWorld())
    {
        UE_LOG(LogTemp, Error,
            TEXT("AttachTool failed: missing ToolAsset or World."));
        return;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = this;

    AEquippableToolBase* ToolToEquip =
        GetWorld()->SpawnActor<AEquippableToolBase>(
            ToolDefinition->ToolAsset,
            GetActorLocation(),
            GetActorRotation(),
            SpawnParams);

    if (!ToolToEquip)
    {
        UE_LOG(LogTemp, Error,
            TEXT("AttachTool failed: could not spawn tool."));
        return;
    }

    FAttachmentTransformRules AttachmentRules(
        EAttachmentRule::SnapToTarget,
        true);

    ToolToEquip->AttachToActor(this, AttachmentRules);

    if (FirstPersonMeshComponent)
    {
        ToolToEquip->AttachToComponent(
            FirstPersonMeshComponent,
            AttachmentRules,
            FName(TEXT("GripPoint")));
    }

    if (ToolToEquip->FirstPersonToolAnim && FirstPersonMeshComponent)
    {
        FirstPersonMeshComponent->SetAnimInstanceClass(
            ToolToEquip->FirstPersonToolAnim->GeneratedClass);
    }

    if (ToolToEquip->ThirdPersonToolAnim && GetMesh())
    {
        GetMesh()->SetAnimInstanceClass(
            ToolToEquip->ThirdPersonToolAnim->GeneratedClass);
    }

    ToolToEquip->OwningCharacter = this;
    EquippedTool = ToolToEquip;
    InventoryComponent->ToolInventory.Add(ToolDefinition);

    AddToolInputMapping(ToolToEquip);

    const UInputAction* ActionToBind = UseAction
        ? UseAction.Get()
        : ToolToEquip->UseAction.Get();

    if (ActionToBind)
    {
        ToolToEquip->BindInputAction(ActionToBind);
    }
    else
    {
        UE_LOG(LogTemp, Error,
            TEXT("Tool input failed: UseAction is missing on both character and tool."));
    }

    if (!ToolToEquip->FirstPersonToolAnim)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("FirstPersonToolAnim is not set on the tool."));
    }

    if (!ToolToEquip->ThirdPersonToolAnim)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("ThirdPersonToolAnim is not set on the tool."));
    }

    if (FirstPersonMeshComponent)
    {
        if (UAnimInstance* AnimInstance =
            FirstPersonMeshComponent->GetAnimInstance())
        {
            FBoolProperty* HasRifleProperty =
                FindFProperty<FBoolProperty>(
                    AnimInstance->GetClass(),
                    TEXT("HasRifle"));

            if (HasRifleProperty)
            {
                HasRifleProperty->SetPropertyValue_InContainer(
                    AnimInstance,
                    true);
                UE_LOG(LogTemp, Warning,
                    TEXT("First-person rifle mode enabled."));
            }
            else
            {
                UE_LOG(LogTemp, Warning,
                    TEXT("HasRifle variable not found in the current AnimBP."));
            }
        }
    }

    UE_LOG(LogTemp, Warning,
        TEXT("Tool equipped: %s"),
        *ToolDefinition->ID.ToString());
}

void AAdventureCharacter::GiveItem(
    UItemDefinition* ItemDefinition)
{
    if (!ItemDefinition)
    {
        return;
    }

    switch (ItemDefinition->ItemType)
    {
    case EItemType::Tool:
    {
        UEquippableToolDefinition* ToolDefinition =
            Cast<UEquippableToolDefinition>(ItemDefinition);

        if (ToolDefinition)
        {
            AttachTool(ToolDefinition);
        }
        else
        {
            UE_LOG(LogTemp, Error,
                TEXT("Item is marked as Tool but is not an EquippableToolDefinition."));
        }

        break;
    }

    case EItemType::Consumable:
        UE_LOG(LogTemp, Warning,
            TEXT("Consumable items are not implemented yet."));
        break;

    default:
        UE_LOG(LogTemp, Warning,
            TEXT("Unknown item type."));
        break;
    }
}
void AAdventureCharacter::AddToolInputMapping(
    AEquippableToolBase* Tool)
{
    if (!Tool)
    {
        UE_LOG(LogTemp, Error,
            TEXT("Input mapping failed: Tool is null."));
        return;
    }

    if (!Tool->ToolMappingContext)
    {
        UE_LOG(LogTemp, Error,
            TEXT("Input mapping failed: ToolMappingContext is not set."));
        return;
    }

    APlayerController* PlayerController =
        Cast<APlayerController>(GetController());

    if (!PlayerController)
    {
        UE_LOG(LogTemp, Error,
            TEXT("Input mapping failed: PlayerController is null."));
        return;
    }

    ULocalPlayer* LocalPlayer =
        PlayerController->GetLocalPlayer();

    if (!LocalPlayer)
    {
        UE_LOG(LogTemp, Error,
            TEXT("Input mapping failed: LocalPlayer is null."));
        return;
    }

    UEnhancedInputLocalPlayerSubsystem* Subsystem =
        LocalPlayer->GetSubsystem<
        UEnhancedInputLocalPlayerSubsystem>();

    if (!Subsystem)
    {
        UE_LOG(LogTemp, Error,
            TEXT("Input mapping failed: EnhancedInput subsystem is null."));
        return;
    }

    Subsystem->AddMappingContext(
        Tool->ToolMappingContext, 1);

    UE_LOG(LogTemp, Warning,
        TEXT("Tool mapping context added successfully."));
}

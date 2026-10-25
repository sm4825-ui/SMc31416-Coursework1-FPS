
#include "EquippableToolBase.h"

#include "EnhancedInputComponent.h"
#include "Engine/Engine.h"
#include "InputAction.h"

AEquippableToolBase::AEquippableToolBase()
{
    PrimaryActorTick.bCanEverTick = false;

    ToolMeshComponent =
        CreateDefaultSubobject<USkeletalMeshComponent>(
            TEXT("ToolMesh"));

    SetRootComponent(ToolMeshComponent);

    ToolMeshComponent->SetCollisionEnabled(
        ECollisionEnabled::NoCollision);

    OwningCharacter = nullptr;
}

void AEquippableToolBase::Use()
{
}

void AEquippableToolBase::BindInputAction(
    const UInputAction* ActionToBind)
{
}

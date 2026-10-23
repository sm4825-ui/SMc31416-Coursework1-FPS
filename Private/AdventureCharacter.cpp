
#include "AdventureCharacter.h"

AAdventureCharacter::AAdventureCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AAdventureCharacter::BeginPlay()
{
    Super::BeginPlay();
}

void AAdventureCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AAdventureCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

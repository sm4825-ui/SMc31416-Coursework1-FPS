

#include "AdventureGameMode.h"

void AAdventureGameMode::StartPlay()
{
    Super::StartPlay();

    check(GEngine != nullptr);

    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Hello World, this is AdventureGameMode!"));
}
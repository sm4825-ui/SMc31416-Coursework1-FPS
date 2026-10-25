
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AdventureGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ADVENTUREGAME_API AAdventureGameMode : public AGameModeBase
{
	GENERATED_BODY()
	virtual void StartPlay() override;
};

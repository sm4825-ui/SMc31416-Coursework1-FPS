#pragma once

#include "CoreMinimal.h"
#include "EquippableToolBase.h"
#include "DartLauncher.generated.h"

class AFirstPersonProjectile;

UCLASS(BlueprintType, Blueprintable)
class ADVENTUREGAME_API ADartLauncher : public AEquippableToolBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
	TSubclassOf<AFirstPersonProjectile> ProjectileClass;

	virtual void Use() override;

	virtual void BindInputAction(
		const UInputAction* InputToBind) override;
};

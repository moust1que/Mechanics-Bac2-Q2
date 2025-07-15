#pragma once

#include "CoreMinimal.h"
#include "../BaseCharacter.h"
#include "Akali.generated.h"

// Forward declaration
class AShurikenProjectile;

UCLASS()
class MECHANICS_API AAkali : public ABaseCharacter {
	GENERATED_BODY()

    public:
        // Blueprint-exposed property to set the Shuriken projectile class for Akali's abilities
        UPROPERTY(EditDefaultsOnly, Category = Abilities) TSubclassOf<AShurikenProjectile> ShurikenProjectileClass;

    protected:
        virtual void BeginPlay() override;

    private:
        UFUNCTION() void UpdateStats() override;
};
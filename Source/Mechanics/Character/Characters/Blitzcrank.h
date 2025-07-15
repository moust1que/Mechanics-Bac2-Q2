#pragma once

#include "CoreMinimal.h"
#include "../BaseCharacter.h"
#include "Blitzcrank.generated.h"

// Forward declaration
class ARocketGrabProjectile;

UCLASS()
class MECHANICS_API ABlitzcrank : public ABaseCharacter {
	GENERATED_BODY()

    public:
        // Class reference for the RocketGrab projectile used by Blitzcrank's ability
        UPROPERTY(EditAnywhere, Category = Abilities) TSubclassOf<ARocketGrabProjectile> RocketGrabProjectileClass;

    protected:
        virtual void BeginPlay() override;

    private:
        UFUNCTION() void UpdateStats() override;
};
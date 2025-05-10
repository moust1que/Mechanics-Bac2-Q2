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
        UPROPERTY(EditAnywhere, Category = Abilities) TSubclassOf<ARocketGrabProjectile> RocketGrabProjectileClass;

    protected:
        virtual void BeginPlay() override;

    private:
        UFUNCTION() void UpdateStats() override;
};
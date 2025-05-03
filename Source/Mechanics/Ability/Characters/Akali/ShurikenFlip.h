#pragma once

#include "CoreMinimal.h"
#include "../../AbilityBase.h"
#include "../../../Character/Characters/Akali.h"
#include "ShurikenFlip.generated.h"

// Forward declaration
class ABaseCharacter;
class AShurikenProjectile;

UCLASS()
class MECHANICS_API UShurikenFlip : public UAbilityBase {
	GENERATED_BODY()

    public:
        UShurikenFlip();
        UFUNCTION() void ActivateAbility() override;
        void UpdateStats() override;
        UFUNCTION() void StartCooldown() override;
        TArray<float> GetArguments() override;

        UFUNCTION() void PerformRecast();
        UFUNCTION() void CancelRecast();

        UFUNCTION() void ResetCooldown();

        UFUNCTION() void OnShurikenHit(AActor* HitActor, FVector HitLocation);

        UPROPERTY(EditAnywhere, Category = Ability) float TotBaseDamage;
        UPROPERTY(EditAnywhere, Category = Ability) float MarkTimer = 3.0f;

        UPROPERTY(EditDefaultsOnly, Category = Ability) TSubclassOf<AShurikenProjectile> ShurikenProjectileClass;

        UPROPERTY() FVector RecastLocation;

        UPROPERTY() AActor* RecastTarget = nullptr;

        FTimerHandle RecastWindowTimer;

        UPROPERTY() AAkali* AkaliCharacter = nullptr;
};
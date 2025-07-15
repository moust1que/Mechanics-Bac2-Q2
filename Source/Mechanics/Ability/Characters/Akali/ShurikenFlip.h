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
        TArray<float> GetArguments() override;
        UFUNCTION() void LaunchAttack() override;

        UFUNCTION() void PerformRecast();
        UFUNCTION() void CancelRecast();

        UFUNCTION() void OnShurikenHit(AActor* HitActor, FVector HitLocation);
        UFUNCTION() void OnShurikenMiss();

        // Total base damage scaled by level
        UPROPERTY(EditAnywhere, Category = Ability) float TotBaseDamage;
        // Duration in seconds for which the recast is possible after hitting an enemy
        UPROPERTY(EditAnywhere, Category = Ability) float MarkTimer = 3.0f;

        // Class reference for the shuriken projectile spawned during LaunchAttack
        UPROPERTY(EditDefaultsOnly, Category = Ability) TSubclassOf<AShurikenProjectile> ShurikenProjectileClass;

        // Location where recast dash will end
        UPROPERTY() FVector RecastLocation;

        // Actor targeted by recast dash
        UPROPERTY() AActor* RecastTarget = nullptr;

        // Timer handle managing the recast window duration
        FTimerHandle RecastWindowTimer;

        // Cached pointer to the owning Akali character for easy access
        UPROPERTY() AAkali* AkaliCharacter = nullptr;

    private:
        // Timer for dash movement during initial attack
        FTimerHandle DashTimerHandle;
        // Start and target locations for dash movement
        FVector DashStartLocation;
        FVector DashTargetLocation;
        // Duration and elapsed time tracking dash progress
        float DashDuration;
        float DashElapsedTime;

        // Timer for dash movement during recast phase
        FTimerHandle RecastDashTimerHandle;
        // Start and target locations for recast dash
        FVector RecastDashStartLocation;
        FVector RecastDashTargetLocation;
        // Duration and elapsed time tracking recast dash progress
        float RecastDashDuration;
        float RecastDashElapsedTime;

        UFUNCTION() void HandleDashTick();
        UFUNCTION() void HandleRecastDashTick();
};
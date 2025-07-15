#pragma once

#include "CoreMinimal.h"
#include "../../AbilityBase.h"
#include "../../../Character/Characters/Akali.h"
#include "PerfectExecution.generated.h"

UCLASS()
class MECHANICS_API UPerfectExecution : public UAbilityBase {
	GENERATED_BODY()

    public:
        UPerfectExecution();
        UFUNCTION() void ActivateAbility() override;
        void UpdateStats() override;
        TArray<float> GetArguments() override;

        UFUNCTION() void PerformRecast();
        UFUNCTION() void CancelRecast();

        // Base damage range used to compute recast scaling (based on enemy HP)
        UPROPERTY(EditAnywhere, Category = Ability) float MinBaseDamage;
        UPROPERTY(EditAnywhere, Category = Ability) float MaxBaseDamage;

        // Reference to the Akali character using the ability
        UPROPERTY() AAkali* AkaliCharacter = nullptr;

        // First dash movement variables
        FVector DashStartLocation;
        FVector DashTargetLocation;
        float DashDuration;
        float DashElapsedTime;

        // Recast movement variables
        FVector RecastStartLocation;
        FVector RecastTargetLocation;
        float RecastDuration;
        float RecastElapsedTime;

        // Timers for dash movement, recast window, and second dash
        FTimerHandle DashTimerHandle;
        FTimerHandle RecastWindowTimer;
        FTimerHandle RecastTimerHandle;

        // Keeps track of enemies already hit to avoid double damage
        TSet<ABaseCharacter*> DamagedEnemies;

        UFUNCTION() void HandleDashTick();
        UFUNCTION() void HandleRecastDashTick();
};
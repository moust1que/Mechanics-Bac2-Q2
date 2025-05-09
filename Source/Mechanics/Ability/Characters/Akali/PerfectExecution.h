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

        UPROPERTY(EditAnywhere, Category = Ability) float MinBaseDamage;
        UPROPERTY(EditAnywhere, Category = Ability) float MaxBaseDamage;

        UPROPERTY() AAkali* AkaliCharacter = nullptr;

        FVector DashStartLocation;
        FVector DashTargetLocation;
        float DashDuration;
        float DashElapsedTime;

        FVector RecastStartLocation;
        FVector RecastTargetLocation;
        float RecastDuration;
        float RecastElapsedTime;

        FTimerHandle DashTimerHandle;
        FTimerHandle RecastWindowTimer;
        FTimerHandle RecastTimerHandle;

        TSet<ABaseCharacter*> DamagedEnemies;

        UFUNCTION() void HandleDashTick();
        UFUNCTION() void HandleRecastDashTick();
};
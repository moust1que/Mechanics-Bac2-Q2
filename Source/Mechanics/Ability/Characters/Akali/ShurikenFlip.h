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
        UFUNCTION() void StartCastTimer(float CastDuration, FName FunctionName) override;
        UFUNCTION() void LaunchAttack() override;
        UFUNCTION() void StartCooldown() override;
        void ResetCooldown() override;

        UFUNCTION() void PerformRecast();
        UFUNCTION() void CancelRecast();

        UFUNCTION() void OnShurikenHit(AActor* HitActor, FVector HitLocation);
        UFUNCTION() void OnShurikenMiss();

        UPROPERTY(EditAnywhere, Category = Ability) float TotBaseDamage;
        UPROPERTY(EditAnywhere, Category = Ability) float MarkTimer = 3.0f;

        UPROPERTY(EditDefaultsOnly, Category = Ability) TSubclassOf<AShurikenProjectile> ShurikenProjectileClass;

        UPROPERTY() FVector RecastLocation;

        UPROPERTY() AActor* RecastTarget = nullptr;

        FTimerHandle RecastWindowTimer;

        UPROPERTY() AAkali* AkaliCharacter = nullptr;

    private:
        FTimerHandle DashTimerHandle;
        FVector DashStartLocation;
        FVector DashTargetLocation;
        float DashDuration;
        float DashElapsedTime;

        FTimerHandle RecastDashTimerHandle;
        FVector RecastDashStartLocation;
        FVector RecastDashTargetLocation;
        float RecastDashDuration;
        float RecastDashElapsedTime;

        UFUNCTION() void HandleDashTick();
        UFUNCTION() void HandleRecastDashTick();
};
#pragma once

#include "CoreMinimal.h"
#include "../../AbilityBase.h"
#include "ShurikenFlip.generated.h"

UCLASS()
class MECHANICS_API UShurikenFlip : public UAbilityBase {
	GENERATED_BODY()

    public:
        UShurikenFlip();
        UFUNCTION() void ActivateAbility() override;
        UFUNCTION() void UpdateStats() override;

        UPROPERTY(EditAnywhere, Category = Ability) float TotBaseDamage;
        UPROPERTY(EditAnywhere, Category = Ability) float BaseRecastCooldown = 3.0f;
        UPROPERTY(EditAnywhere, Category = Ability) float BaseRecastRessourceCost = 0.0f;
        UPROPERTY(EditAnywhere, Category = Ability) float RecastBaseDamage;
        UPROPERTY(EditAnywhere, Category = Ability) float RecastAbilityDamage;
        UPROPERTY(EditAnywhere, Category = Ability) float RecastAbilityPower;
        UPROPERTY(EditAnywhere, Category = Ability) float RecastTotalDamage;
};
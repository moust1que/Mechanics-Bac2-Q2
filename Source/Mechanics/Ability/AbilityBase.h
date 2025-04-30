#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AbilityBase.generated.h"

// Forward declaration
class ABaseCharacter;

UCLASS()
class MECHANICS_API UAbilityBase : public UObject {
	GENERATED_BODY()

    public:
        UPROPERTY(EditAnywhere, Category = Ability) ABaseCharacter* CurCharacter;
        UPROPERTY(EditAnywhere, Category = Ability) int Level;
        UPROPERTY(EditAnywhere, Category = Ability) int MaxLevel;
        UPROPERTY(EditAnywhere, Category = Ability) float BaseCooldown;
        UPROPERTY(EditAnywhere, Category = Ability) float Cooldown;
        UPROPERTY(EditAnywhere, Category = Ability) float BaseRessourceCost;
        UPROPERTY(EditAnywhere, Category = Ability) float RessourceCost;
        UPROPERTY(EditAnywhere, Category = Ability) float BaseDamage;
        UPROPERTY(EditAnywhere, Category = Ability) float AbilityDamage;
        UPROPERTY(EditAnywhere, Category = Ability) float AbilityPower;
        UPROPERTY(EditAnywhere, Category = Ability) float TotalDamage;

        virtual void ActivateAbility() {};
        virtual void UpdateStats() {};
};
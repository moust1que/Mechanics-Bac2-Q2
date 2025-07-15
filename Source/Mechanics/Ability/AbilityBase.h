#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AbilityBase.generated.h"

// Forward declaration
class ABaseCharacter;
struct FTimerHandle;

UCLASS()
class MECHANICS_API UAbilityBase : public UObject {
	GENERATED_BODY()

    public:
        // The character currently using the ability
        UPROPERTY(EditAnywhere, Category = Ability) ABaseCharacter* CurCharacter;

        // General ability stats
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ability) int Level;
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ability) int AbilityID;
        UPROPERTY(EditAnywhere, Category = Ability) int MaxLevel;
        // Cooldown management
        UPROPERTY(EditAnywhere, Category = Ability) float BaseCooldown;
        UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability) float Cooldown;
        UPROPERTY(EditAnywhere, Category = Ability) bool IsOnCooldown = false;
        // Ressource cost
        UPROPERTY(EditAnywhere, Category = Ability) float BaseRessourceCost;
        UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability) float RessourceCost;
        // Damage
        UPROPERTY(EditAnywhere, Category = Ability) float BaseDamage;
        UPROPERTY(EditAnywhere, Category = Ability) float AbilityDamage;
        UPROPERTY(EditAnywhere, Category = Ability) float AbilityPower;
        UPROPERTY(EditAnywhere, Category = Ability) float TotalDamage;

        // Recast mechanics
        UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability) bool CanRecast = false;
        UPROPERTY(EditAnywhere, Category = Ability) float BaseRecastCooldown;
        UPROPERTY(EditAnywhere, Category = Ability) float BaseRecastRessourceCost;
        UPROPERTY(EditAnywhere, Category = Ability) float RecastBaseDamage;
        UPROPERTY(EditAnywhere, Category = Ability) float RecastAbilityDamage;
        UPROPERTY(EditAnywhere, Category = Ability) float RecastAbilityPower;
        UPROPERTY(EditAnywhere, Category = Ability) float RecastTotalDamage;

        // Additional arguments for the tooltips
        UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Arguments) float Arg1;
        UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Arguments) float Arg2;
        UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Arguments) float Arg3;

        // Cooldown timer handle
        FTimerHandle CooldownTimer;

        // Ability usability flags
        bool CanBeUsed = true;
        bool NeedEnemyTarget = false;
        bool NeedEnemytargetRecast = false;

        // Current enemy target
        AActor* EnemyTarget = nullptr;

        // Effective ability range
        float AbilityRange;

        virtual void ActivateAbility() {};
        UFUNCTION(BlueprintCallable) virtual void UpdateStats() {};
        void StartCooldown(float CooldowToUse, bool IsSkipable = false);
        UFUNCTION(BlueprintCallable, Category = Ability) void ResetCooldown();
        UFUNCTION(BlueprintCallable) virtual TArray<float> GetArguments() {return {Arg1, Arg2, Arg3};};
        void StartCastTimer(float CastDuration, FName FunctionName);
        virtual void LaunchAttack() {};
};
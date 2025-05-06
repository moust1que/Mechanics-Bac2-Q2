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
        UPROPERTY(EditAnywhere, Category = Ability) ABaseCharacter* CurCharacter;

        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ability) int Level;
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ability) int AbilityID;
        UPROPERTY(EditAnywhere, Category = Ability) int MaxLevel;
        UPROPERTY(EditAnywhere, Category = Ability) float BaseCooldown;
        UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability) float Cooldown;
        UPROPERTY(EditAnywhere, Category = Ability) bool IsOnCooldown = false;
        UPROPERTY(EditAnywhere, Category = Ability) float BaseRessourceCost;
        UPROPERTY(EditAnywhere, Category = Ability) float RessourceCost;
        UPROPERTY(EditAnywhere, Category = Ability) float BaseDamage;
        UPROPERTY(EditAnywhere, Category = Ability) float AbilityDamage;
        UPROPERTY(EditAnywhere, Category = Ability) float AbilityPower;
        UPROPERTY(EditAnywhere, Category = Ability) float TotalDamage;

        UPROPERTY(EditAnywhere, Category = Ability) bool CanRecast = false;
        UPROPERTY(EditAnywhere, Category = Ability) float BaseRecastCooldown;
        UPROPERTY(EditAnywhere, Category = Ability) float BaseRecastRessourceCost;
        UPROPERTY(EditAnywhere, Category = Ability) float RecastBaseDamage;
        UPROPERTY(EditAnywhere, Category = Ability) float RecastAbilityDamage;
        UPROPERTY(EditAnywhere, Category = Ability) float RecastAbilityPower;
        UPROPERTY(EditAnywhere, Category = Ability) float RecastTotalDamage;

        UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Arguments) float Arg1;
        UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Arguments) float Arg2;
        UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Arguments) float Arg3;

        FTimerHandle CooldownTimer;

        virtual void ActivateAbility() {};
        UFUNCTION(BlueprintCallable) virtual void UpdateStats() {};
        virtual void StartCooldown() {};
        UFUNCTION(BlueprintCallable, Category = Ability) virtual void ResetCooldown() {};
        UFUNCTION(BlueprintCallable) virtual TArray<float> GetArguments() {return {Arg1, Arg2, Arg3};};
        virtual void StartCastTimer(float CastDuration, FName FunctionName) {};
        virtual void LaunchAttack() {};
};
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Ability/AbilityBase.h"
#include "MainHUD.generated.h"

UCLASS()
class MECHANICS_API UMainHUD : public UUserWidget {
	GENERATED_BODY()

    public:
        UFUNCTION(BlueprintImplementableEvent) void StartCooldown(UAbilityBase* Ability, float Cooldown);
        UFUNCTION(BlueprintImplementableEvent) void ResetCooldown(UAbilityBase* Ability);
        UFUNCTION(BlueprintImplementableEvent) void UpdateSpellRecastDisplay(UAbilityBase* Ability);
        UFUNCTION(BlueprintImplementableEvent) void UpdateResourceOnChange();
        UFUNCTION(BlueprintImplementableEvent) void UpdateHealthOnChange();

        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ability) bool AbilityClicked = false;
};
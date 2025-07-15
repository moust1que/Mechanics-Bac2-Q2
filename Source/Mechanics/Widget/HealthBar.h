#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBar.generated.h"

// Enum representing different resource types that the health bar widget can display
UENUM(BlueprintType) enum class ERessourceType : uint8 {
    Mana, Energy, Timer
};

UCLASS()
class MECHANICS_API UHealthBar : public UUserWidget {
	GENERATED_BODY()

    public:
        UFUNCTION(BlueprintImplementableEvent) void StartHealthReload();
        UFUNCTION(BlueprintImplementableEvent) void DisplayResourceBar(ERessourceType ResourceType);
        UFUNCTION(BlueprintImplementableEvent) void UpdateResourceOnChange( float CurrentRessource, float MaxRessource);
        UFUNCTION(BlueprintImplementableEvent) void UpdateHealthOnChange(float CurrentHealth, float MaxHealth);
        UFUNCTION(BlueprintImplementableEvent) void UpdateLevel();
};
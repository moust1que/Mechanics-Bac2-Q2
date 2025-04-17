#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

// Forward declaration
class UAbilityBase;
struct FInputActionInstance;

UCLASS()
class MECHANICS_API ABaseCharacter : public ACharacter {
	GENERATED_BODY()

    public:
        UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Abilities) TMap<FName, TSubclassOf<UAbilityBase>> AbilityMap; // "Q", "W", "E", "R"

        UPROPERTY() TMap<FName, UAbilityBase*> InstantiatedAbilities;

        void HandleFunctionCall(FName functionName, const FInputActionInstance& Instance);

        void ActivateAbility(FName AbilitySlot); // "Q", "W", etc.

    protected:
        // Called when the game starts or when spawned
        virtual void BeginPlay() override;
};
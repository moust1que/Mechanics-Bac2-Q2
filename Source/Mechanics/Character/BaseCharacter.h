#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

// Forward declaration
class UAbilityBase;
struct FInputActionInstance;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class MECHANICS_API ABaseCharacter : public ACharacter {
	GENERATED_BODY()

    public:
        ABaseCharacter();

        UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Abilities) TMap<FName, TSubclassOf<UAbilityBase>> AbilityMap; // "Q", "W", "E", "R"

        UPROPERTY() TMap<FName, UAbilityBase*> InstantiatedAbilities;

        void HandleFunctionCall(FName functionName, const FInputActionInstance& Instance);

        void ActivateAbility(FName AbilitySlot); // "Q", "W", etc.

        UPROPERTY(EditAnywhere, Category = Camera) float CameraZoomSpeed = 100.0f;
        UPROPERTY(EditAnywhere, Category = Camera) float CameraZoomMin = 1200.0f;
        UPROPERTY(EditAnywhere, Category = Camera) float CameraZoomMax = 2500.0f;

    protected:
        // Called when the game starts or when spawned
        virtual void BeginPlay() override;

    private:
        USpringArmComponent* SpringArmComponent;
        UCameraComponent* CameraComponent;

        UFUNCTION() void Ability1();
        UFUNCTION() void Ability2();
        UFUNCTION() void Ability3();
        UFUNCTION() void Ability4();
        UFUNCTION() void ZoomCamera(const FInputActionInstance& Instance);
};
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputTriggers.h"
#include "PlayerController_Mechanics.generated.h"

// Forward declaration
class UInputMappingContext;
class UInputAction;
struct FInputActionInstance;
class ABaseCharacter;

// Struct to hold the names of functions to call for each input trigger event
USTRUCT(BlueprintType) struct FInputFunctionBinding {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite) FName OnStarted;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FName OnTriggered;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FName OnGoing;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FName OnCanceled;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FName OnCompleted;
};

UCLASS()
class MECHANICS_API APlayerController_Mechanics : public APlayerController {
	GENERATED_BODY()

    public:
        APlayerController_Mechanics();

        // Input Mapping Context asset for enhanced input system
        UPROPERTY(EditAnywhere, Category = Input) TSoftObjectPtr<UInputMappingContext> IMC_Mechanics;

        // Array of input actions that this controller listens to
        UPROPERTY(EditAnywhere, Category = Input) TArray<UInputAction*> InputActions = {};
        
        // Mapping from InputAction to the set of function bindings for input events
        UPROPERTY(EditAnywhere, Category = Input) TMap<UInputAction*, FInputFunctionBinding> FunctionBindings;

        // Array of character classes to switch between
        UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Characters) TArray<TSubclassOf<ABaseCharacter>> Characters;

        UFUNCTION(BlueprintCallable) ABaseCharacter* SetCharacter();

        // Widgets for custom mouse cursors
        UPROPERTY(EditAnywhere, Category = Cursors) UUserWidget* DefaultCursorBrush;
        UPROPERTY(EditAnywhere, Category = Cursors) UUserWidget* TargetingCursorBrush;
        UPROPERTY(EditAnywhere, Category = Cursors) UUserWidget* TargetingEnemyCursorBrush;

        virtual void Tick(float DeltaTime) override;

    protected:
        virtual void SetupInputComponent() override;
        virtual void BeginPlay() override;

    private:
        void DynamicInputHandler(FName FunctionName, const FInputActionInstance& Instance);
        void OnActionStarted(const FInputActionInstance& Instance);
        void OnActionTriggered(const FInputActionInstance& Instance);
        void OnActionGoing(const FInputActionInstance& Instance);
        void OnActionCanceled(const FInputActionInstance& Instance);
        void OnActionCompleted(const FInputActionInstance& Instance);

        // List of input trigger events to bind to input actions
        TArray<ETriggerEvent> EventsToBind = {
            ETriggerEvent::Started,
            ETriggerEvent::Triggered,
            ETriggerEvent::Ongoing,
            ETriggerEvent::Canceled,
            ETriggerEvent::Completed
        };
};
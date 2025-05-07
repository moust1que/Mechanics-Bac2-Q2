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

        UPROPERTY(EditAnywhere, Category = Input) TSoftObjectPtr<UInputMappingContext> IMC_Mechanics;

        UPROPERTY(EditAnywhere, Category = Input) TArray<UInputAction*> InputActions = {};
        
        UPROPERTY(EditAnywhere, Category = Input) TMap<UInputAction*, FInputFunctionBinding> FunctionBindings;

        UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Characters) TArray<TSubclassOf<ABaseCharacter>> Characters;

        UFUNCTION(BlueprintCallable) ABaseCharacter* SetCharacter();

        virtual void Tick(float DeltaTime) override;

    protected:
        virtual void SetupInputComponent() override;

    private:
        void DynamicInputHandler(FName FunctionName, const FInputActionInstance& Instance);
        void OnActionStarted(const FInputActionInstance& Instance);
        void OnActionTriggered(const FInputActionInstance& Instance);
        void OnActionGoing(const FInputActionInstance& Instance);
        void OnActionCanceled(const FInputActionInstance& Instance);
        void OnActionCompleted(const FInputActionInstance& Instance);

        TArray<ETriggerEvent> EventsToBind = {
            ETriggerEvent::Started,
            ETriggerEvent::Triggered,
            ETriggerEvent::Ongoing,
            ETriggerEvent::Canceled,
            ETriggerEvent::Completed
        };
};
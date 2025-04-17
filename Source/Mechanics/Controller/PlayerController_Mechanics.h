#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerController_Mechanics.generated.h"

// Forward declaration
class UNiagaraSystem;
class UInputMappingContext;
class UInputAction;
struct FInputActionInstance;

UCLASS()
class MECHANICS_API APlayerController_Mechanics : public APlayerController {
	GENERATED_BODY()

    public:
        APlayerController_Mechanics();

        UPROPERTY(EditAnywhere, Category = Input) TSoftObjectPtr<UInputMappingContext> IMC_Mechanics;

        UPROPERTY(EditAnywhere, Category = Input) TArray<UInputAction*> InputActions = {};
        UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true")) UInputAction* SetDestinationClickAction;

        UPROPERTY(EditAnywhere, Category = Input) TMap<FName, FName> ActionFunctionMapping = {};

        UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input) UNiagaraSystem* FXCursor;

    protected:
        virtual void BeginPlay() override;
        virtual void SetupInputComponent() override;

        void OnInputStarted();
        void OnSetDestinationTriggered();
        void OnSetDestinationReleased();

    private:
        FVector CachedDestination;

        float ShortPressThreshold = 0.3f;
        float FollowTime;

        void DynamicInputHandler(const FInputActionInstance& Instance);
};
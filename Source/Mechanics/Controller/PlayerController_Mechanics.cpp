#include "PlayerController_Mechanics.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "../Character/BaseCharacter.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraFunctionLibrary.h"

APlayerController_Mechanics::APlayerController_Mechanics() {
    bShowMouseCursor = true;
    DefaultMouseCursor = EMouseCursor::Default;
    CachedDestination = FVector::ZeroVector;
    FollowTime = 0.0f;
}

void APlayerController_Mechanics::BeginPlay() {
    Super::BeginPlay();
}

void APlayerController_Mechanics::SetupInputComponent() {
    Super::SetupInputComponent();

    if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer())) {
        Subsystem->AddMappingContext(IMC_Mechanics.LoadSynchronous(), 0);
    }

    if(UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent)) {
        for(UInputAction* action : InputActions) {
            if(action) {
                EnhancedInputComponent->BindAction(action, ETriggerEvent::Triggered, this, &APlayerController_Mechanics::DynamicInputHandler);
            }
        }

        EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &APlayerController_Mechanics::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &APlayerController_Mechanics::OnSetDestinationTriggered);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &APlayerController_Mechanics::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &APlayerController_Mechanics::OnSetDestinationReleased);
    }
}

void APlayerController_Mechanics::DynamicInputHandler(const FInputActionInstance& Instance) {
    FName actionName = Instance.GetSourceAction()->GetFName();

    if(ActionFunctionMapping.Contains(actionName)) {
        FName functionName = ActionFunctionMapping[actionName];

        if(ABaseCharacter* character = Cast<ABaseCharacter>(GetPawn())){
            character->HandleFunctionCall(functionName, Instance);
        }
    }
}

void APlayerController_Mechanics::OnInputStarted() {
    StopMovement();
}

void APlayerController_Mechanics::OnSetDestinationTriggered() {
    FollowTime += GetWorld()->GetDeltaSeconds();

    FHitResult Hit;
    bool bHitSuccessful = false;
    bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);

    if(bHitSuccessful) {
		CachedDestination = Hit.Location;
	}
	
	APawn* ControlledPawn = GetPawn();
	if(ControlledPawn != nullptr) {
		FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
		ControlledPawn->AddMovementInput(WorldDirection, 1.0, false);
	}
}

void APlayerController_Mechanics::OnSetDestinationReleased() {
    if(FollowTime <= ShortPressThreshold) {
        UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.0f, 1.0f, 1.0f), true, true, ENCPoolMethod::None, true);
    }

	FollowTime = 0.f;
}
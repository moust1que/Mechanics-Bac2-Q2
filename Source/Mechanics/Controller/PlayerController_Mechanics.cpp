#include "PlayerController_Mechanics.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "../Character/BaseCharacter.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "../Widget/MainHUD.h"
#include "NiagaraFunctionLibrary.h"
#include "../Ability/AbilityTargetingIndicator.h"

APlayerController_Mechanics::APlayerController_Mechanics() {
    // Show the mouse cursor by default
    bShowMouseCursor = true;
    // Enable mouse click events for this controller
    bEnableClickEvents = true;
    // Enable mouse over events for this controller
    bEnableMouseOverEvents = true;
}

void APlayerController_Mechanics::BeginPlay() {
    Super::BeginPlay();

    // Set a custom mouse cursor widget using a predefined brush
    SetMouseCursorWidget(EMouseCursor::Default, DefaultCursorBrush);
}

void APlayerController_Mechanics::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    // Cast the possessed Pawn to the custom BaseCharacter class
    ABaseCharacter* CurCharacter = Cast<ABaseCharacter>(GetPawn());
    if(!CurCharacter) return; // Early out if no character possessed

    // Get the targeting indicator owned by the character
    AAbilityTargetingIndicator* CurrentTargetIndicator = CurCharacter->CurrentTargetIndicator;
    if(!CurrentTargetIndicator) return; // Early out if no indicator found

    // Check if the character is currently in ability targeting mode
    if(CurCharacter->IsInAbilityTargeting()) {
        FVector2D MousePosition;
        // Get the current mouse screen position
        if(GetMousePosition(MousePosition.X, MousePosition.Y)) {
            // Convert mouse screen position to a world direction and origin
            FVector WorldOrigin, WorldDirection;
            DeprojectScreenPositionToWorld(MousePosition.X, MousePosition.Y, WorldOrigin, WorldDirection);
            
            FVector Start = WorldOrigin;
            FVector End = Start + WorldDirection * 10000.0f; // Trace far into the world

            // Perform a line trace to find collision point in the world
            FHitResult Hit;
            FCollisionQueryParams Params;
            Params.AddIgnoredActor(this); // Ignore self in trace
            
            // Trace against a specific collision channel
            if(GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_GameTraceChannel1, Params)) {
                // Calculate direction vector from character to hit location, flattening Z axis
                FVector Direction = Hit.ImpactPoint - CurCharacter->GetActorLocation();
                Direction.Z = 0.0f;

                // Update the targeting indicator's direction based on calculated vector
                CurrentTargetIndicator->UpdateIndicatorDirection(Direction);
            }
        }
    }
}

// Called to bind functionality to input
void APlayerController_Mechanics::SetupInputComponent() {
    Super::SetupInputComponent();

    // Get the Enhanced Input subsystem from the local player to add input mapping context
    if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer())) {
        // Add the Mechanics input mapping context with priority 0
        Subsystem->AddMappingContext(IMC_Mechanics.LoadSynchronous(), 0);
    }

    // Cast the InputComponent to the enhanced input component
    if(UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent)) {
        // Loop through all input actions to bind them dynamically
        for(UInputAction* action : InputActions) {
            if(!action) continue; // Skip null actions

            // Bind all relevant trigger events to their handler functions
            EnhancedInputComponent->BindAction(action, ETriggerEvent::Started, this, &APlayerController_Mechanics::OnActionStarted);
            EnhancedInputComponent->BindAction(action, ETriggerEvent::Triggered, this, &APlayerController_Mechanics::OnActionTriggered);
            EnhancedInputComponent->BindAction(action, ETriggerEvent::Completed, this, &APlayerController_Mechanics::OnActionCompleted);
            EnhancedInputComponent->BindAction(action, ETriggerEvent::Canceled, this, &APlayerController_Mechanics::OnActionCanceled);
            EnhancedInputComponent->BindAction(action, ETriggerEvent::Ongoing, this, &APlayerController_Mechanics::OnActionGoing);
        }
    }
}

// Handler called when an input action is started
void APlayerController_Mechanics::OnActionStarted(const FInputActionInstance& Instance) {
    // Retrieve the input action from the instance
    const UInputAction* Action = Instance.GetSourceAction();
    if(!Action || !FunctionBindings.Contains(Action)) return; // Return if action invalid or no binding

    // Get the function name mapped to the Started event of this action
    FName FunctionToCall = FunctionBindings[Action].OnStarted;

    // Forward the input event to the character via dynamic function call
    DynamicInputHandler(FunctionToCall, Instance);
}

// Handler called when an input action is triggered
void APlayerController_Mechanics::OnActionTriggered(const FInputActionInstance& Instance) {
    const UInputAction* Action = Instance.GetSourceAction();
    if(!Action || !FunctionBindings.Contains(Action)) return;

    FName FunctionToCall = FunctionBindings[Action].OnTriggered;

    DynamicInputHandler(FunctionToCall, Instance);
}

// Handler called when an input action is completed
void APlayerController_Mechanics::OnActionCompleted(const FInputActionInstance& Instance) {
    const UInputAction* Action = Instance.GetSourceAction();
    if(!Action || !FunctionBindings.Contains(Action)) return;

    FName FunctionToCall = FunctionBindings[Action].OnCompleted;

    DynamicInputHandler(FunctionToCall, Instance);
}

// Handler called when an input action is canceled
void APlayerController_Mechanics::OnActionCanceled(const FInputActionInstance& Instance) {
    const UInputAction* Action = Instance.GetSourceAction();
    if(!Action || !FunctionBindings.Contains(Action)) return;

    FName FunctionToCall = FunctionBindings[Action].OnCanceled;

    DynamicInputHandler(FunctionToCall, Instance);
}

// Handler called for ongoing input action events
void APlayerController_Mechanics::OnActionGoing(const FInputActionInstance& Instance) {
    const UInputAction* Action = Instance.GetSourceAction();
    if(!Action || !FunctionBindings.Contains(Action)) return;

    FName FunctionToCall = FunctionBindings[Action].OnGoing;

    DynamicInputHandler(FunctionToCall, Instance);
}

// Helper function that calls the mapped function on the possessed character
void APlayerController_Mechanics::DynamicInputHandler(FName FunctionName, const FInputActionInstance& Instance) {
    if(FunctionName.IsNone()) return; // Skip if no function specified

    // Cast the possessed pawn to BaseCharacter
    if(ABaseCharacter* character = Cast<ABaseCharacter>(GetPawn())) {
        // Forward the function call along with input instance to the character
        character->HandleFunctionCall(FunctionName, Instance);
    }
}

// Swap the controlled character between two character classes and maintain state
ABaseCharacter* APlayerController_Mechanics::SetCharacter() {
    // Get the currently controlled character
    ACharacter* CurCharacter = GetCharacter();
    ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(CurCharacter);
    
    // Save the current location and rotation to spawn the new character in the same place
    FVector NewSpawnLocation = CurCharacter->GetActorLocation();
    FRotator NewSpawnRotation = CurCharacter->GetActorRotation();

    TSubclassOf<ACharacter> NewClass = nullptr;

    // Switch between two character classes based on the current possessed character class
    if(CurCharacter->IsA(Characters[0])) {
        NewClass = Characters[1];
    }else if(CurCharacter->IsA(Characters[1])) {
        NewClass = Characters[0];
    }

    // Preserve the HUD widget from the current character to assign to the new character
    UMainHUD* MainHUD = Cast<UMainHUD>(BaseCharacter->HUDWidget);

    // Preserve cooldown auto-refresh state
    bool KeepRefreshStatus = BaseCharacter->AutoRefreshCooldowns;

    // Destroy the current character actor
    CurCharacter->Destroy();

    // Setup spawn parameters for the new character
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    SpawnParams.Owner = this;
    
    // Spawn the new character of the other class at the saved location and rotation
    ABaseCharacter* NewCharacter = GetWorld()->SpawnActor<ABaseCharacter>(NewClass, NewSpawnLocation, NewSpawnRotation, SpawnParams);

    // Restore HUD and cooldown refresh status on the new character
    NewCharacter->HUDWidget = MainHUD;
    NewCharacter->AutoRefreshCooldowns = KeepRefreshStatus;
    
    // Possess the newly spawned character
    Possess(NewCharacter);

    // Return the new character pointer for reference
    return NewCharacter;
}
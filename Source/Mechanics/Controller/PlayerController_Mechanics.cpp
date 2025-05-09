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
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;
}

void APlayerController_Mechanics::BeginPlay() {
    Super::BeginPlay();

    SetMouseCursorWidget(EMouseCursor::Default, DefaultCursorBrush);
}

void APlayerController_Mechanics::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    ABaseCharacter* CurCharacter = Cast<ABaseCharacter>(GetPawn());
    if(!CurCharacter) return;

    AAbilityTargetingIndicator* CurrentTargetIndicator = CurCharacter->CurrentTargetIndicator;
    if(!CurrentTargetIndicator) return;

    if(CurCharacter->IsInAbilityTargeting()) {
        FVector2D MousePosition;
        if(GetMousePosition(MousePosition.X, MousePosition.Y)) {
            FVector WorldOrigin, WorldDirection;
            DeprojectScreenPositionToWorld(MousePosition.X, MousePosition.Y, WorldOrigin, WorldDirection);
            
            FVector Start = WorldOrigin;
            FVector End = Start + WorldDirection * 10000.0f;

            FHitResult Hit;
            FCollisionQueryParams Params;
            Params.AddIgnoredActor(this);
            
            if(GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_GameTraceChannel1, Params)) {
                FVector Direction = Hit.ImpactPoint - CurCharacter->GetActorLocation();
                Direction.Z = 0.0f;

                CurrentTargetIndicator->UpdateIndicatorDirection(Direction);
            }
        }
    }
}

void APlayerController_Mechanics::SetupInputComponent() {
    Super::SetupInputComponent();

    if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer())) {
        Subsystem->AddMappingContext(IMC_Mechanics.LoadSynchronous(), 0);
    }

    if(UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent)) {
        for(UInputAction* action : InputActions) {
            if(!action) continue;

            EnhancedInputComponent->BindAction(action, ETriggerEvent::Started, this, &APlayerController_Mechanics::OnActionStarted);
            EnhancedInputComponent->BindAction(action, ETriggerEvent::Triggered, this, &APlayerController_Mechanics::OnActionTriggered);
            EnhancedInputComponent->BindAction(action, ETriggerEvent::Completed, this, &APlayerController_Mechanics::OnActionCompleted);
            EnhancedInputComponent->BindAction(action, ETriggerEvent::Canceled, this, &APlayerController_Mechanics::OnActionCanceled);
            EnhancedInputComponent->BindAction(action, ETriggerEvent::Ongoing, this, &APlayerController_Mechanics::OnActionGoing);
        }
    }
}

void APlayerController_Mechanics::OnActionStarted(const FInputActionInstance& Instance) {
    const UInputAction* Action = Instance.GetSourceAction();
    if(!Action || !FunctionBindings.Contains(Action)) return;

    FName FunctionToCall = FunctionBindings[Action].OnStarted;

    DynamicInputHandler(FunctionToCall, Instance);
}

void APlayerController_Mechanics::OnActionTriggered(const FInputActionInstance& Instance) {
    const UInputAction* Action = Instance.GetSourceAction();
    if(!Action || !FunctionBindings.Contains(Action)) return;

    FName FunctionToCall = FunctionBindings[Action].OnTriggered;

    DynamicInputHandler(FunctionToCall, Instance);
}

void APlayerController_Mechanics::OnActionCompleted(const FInputActionInstance& Instance) {
    const UInputAction* Action = Instance.GetSourceAction();
    if(!Action || !FunctionBindings.Contains(Action)) return;

    FName FunctionToCall = FunctionBindings[Action].OnCompleted;

    DynamicInputHandler(FunctionToCall, Instance);
}

void APlayerController_Mechanics::OnActionCanceled(const FInputActionInstance& Instance) {
    const UInputAction* Action = Instance.GetSourceAction();
    if(!Action || !FunctionBindings.Contains(Action)) return;

    FName FunctionToCall = FunctionBindings[Action].OnCanceled;

    DynamicInputHandler(FunctionToCall, Instance);
}

void APlayerController_Mechanics::OnActionGoing(const FInputActionInstance& Instance) {
    const UInputAction* Action = Instance.GetSourceAction();
    if(!Action || !FunctionBindings.Contains(Action)) return;

    FName FunctionToCall = FunctionBindings[Action].OnGoing;

    DynamicInputHandler(FunctionToCall, Instance);
}

void APlayerController_Mechanics::DynamicInputHandler(FName FunctionName, const FInputActionInstance& Instance) {
    if(FunctionName.IsNone()) return;

    if(ABaseCharacter* character = Cast<ABaseCharacter>(GetPawn())) {
        character->HandleFunctionCall(FunctionName, Instance);
    }
}

ABaseCharacter* APlayerController_Mechanics::SetCharacter() {
    ACharacter* CurCharacter = GetCharacter();
    ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(CurCharacter);
    
    FVector NewSpawnLocation = CurCharacter->GetActorLocation();
    FRotator NewSpawnRotation = CurCharacter->GetActorRotation();

    TSubclassOf<ACharacter> NewClass = nullptr;

    if(CurCharacter->IsA(Characters[0])) {
        NewClass = Characters[1];
    }else if(CurCharacter->IsA(Characters[1])) {
        NewClass = Characters[0];
    }

    
    UMainHUD* MainHUD = Cast<UMainHUD>(BaseCharacter->HUDWidget);

    bool KeepRefreshStatus = BaseCharacter->AutoRefreshCooldowns;

    CurCharacter->Destroy();

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    SpawnParams.Owner = this;
    
    ABaseCharacter* NewCharacter = GetWorld()->SpawnActor<ABaseCharacter>(NewClass, NewSpawnLocation, NewSpawnRotation, SpawnParams);

    NewCharacter->HUDWidget = MainHUD;
    NewCharacter->AutoRefreshCooldowns = KeepRefreshStatus;
    
    Possess(NewCharacter);

    return NewCharacter;
}
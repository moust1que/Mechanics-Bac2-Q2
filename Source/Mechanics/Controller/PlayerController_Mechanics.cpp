#include "PlayerController_Mechanics.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "../Character/BaseCharacter.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "../Widget/MainHUD.h"
#include "NiagaraFunctionLibrary.h"

APlayerController_Mechanics::APlayerController_Mechanics() {
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;
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
    WasCancellingAbility = false;

    if(ABaseCharacter* character = Cast<ABaseCharacter>(GetPawn())) {
        if(character->IsInAbilityTargeting()) {
            character->CancelAttack();
            WasCancellingAbility = true;
            return;
        }
    }

    StopMovement();
}

void APlayerController_Mechanics::OnSetDestinationTriggered() {
    if(WasCancellingAbility) return;
    
    if(ABaseCharacter* character = Cast<ABaseCharacter>(GetPawn())) {
        if(character->IsInAbilityTargeting()) return;
    }

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
    if(WasCancellingAbility) {
        WasCancellingAbility = false;
        return;
    }

    if(ABaseCharacter* character = Cast<ABaseCharacter>(GetPawn())) {
        if(character->IsInAbilityTargeting()) return;
    }

    if(FollowTime <= ShortPressThreshold) {
        UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.0f, 1.0f, 1.0f), true, true, ENCPoolMethod::None, true);
    }

	FollowTime = 0.f;
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

    CurCharacter->Destroy();

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    SpawnParams.Owner = this;
    
    ABaseCharacter* NewCharacter = GetWorld()->SpawnActor<ABaseCharacter>(NewClass, NewSpawnLocation, NewSpawnRotation, SpawnParams);

    NewCharacter->HUDWidget = MainHUD;
    
    Possess(NewCharacter);

    return NewCharacter;
}
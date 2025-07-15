#include "BaseCharacter.h"
#include "../Ability/AbilityBase.h"
#include "InputAction.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "../Ability/AbilityTargetingIndicator.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "../Controller/PlayerController_Mechanics.h"

ABaseCharacter::ABaseCharacter() {
    // Create and configure spring arm component
    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArmComponent->SetupAttachment(RootComponent);
    SpringArmComponent->TargetArmLength = 2000.f;
    SpringArmComponent->bDoCollisionTest = false;
    SpringArmComponent->bUsePawnControlRotation = false;
    SpringArmComponent->bInheritPitch = false;
    SpringArmComponent->bInheritRoll = false;
    SpringArmComponent->bInheritYaw = false;

    // Create and configure camera component attached to spring arm
    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
    CameraComponent->FieldOfView = 60.0f;
    CameraComponent->bUsePawnControlRotation = false;

    // Lock mesh rotation on X and Y axes
    GetMesh()->SetConstraintMode(EDOFMode::SixDOF);
    GetMesh()->BodyInstance.bLockXRotation = true;
    GetMesh()->BodyInstance.bLockYRotation = true;

    // Initialize character level and movement targets
    Level = 1;
    CachedDestination = FVector::ZeroVector;
    FollowTime = 0.0f;
}

void ABaseCharacter::BeginPlay() {
	Super::BeginPlay();

    // Find widget component and cast to health bar widget if present
    UWidgetComponent* WidgetComponent = Cast<UWidgetComponent>(GetComponentByClass(UWidgetComponent::StaticClass()));

    if(WidgetComponent) {
        HealthBarWidget = Cast<UHealthBar>(WidgetComponent->GetUserWidgetObject());
    }

    // Disable controller rotation influence on character rotation
    bUseControllerRotationYaw = false;
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;

    // Set spring arm rotation to look downwards at 45 degrees
    SpringArmComponent->SetRelativeRotation(FRotator(-45.0f, 0.f, 0.f));

    // Instantiate abilities from AbilityMap and update their stats
    for(const auto& AbilityPair : AbilityMap) {
        if(AbilityPair.Value) {
            UAbilityBase* InstantiatedAbility = NewObject<UAbilityBase>(this, AbilityPair.Value);
            InstantiatedAbility->CurCharacter = this;
            InstantiatedAbility->UpdateStats();
            InstantiatedAbilities.Add(AbilityPair.Key, InstantiatedAbility);
        }
    }
}

void ABaseCharacter::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    // Smoothly rotate character to target rotation if needed
    if(ShouldRotate) {
        FRotator CurrentRotation = GetActorRotation();
        FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, RotationSpeed);
        SetActorRotation(NewRotation);

        // Stop rotating once close enough to target rotation
        if(NewRotation.Equals(TargetRotation, 1.0f)) {
            ShouldRotate = false;
        }
    }

    // Update target indicator position and rotation if active
    if(CurrentTargetIndicator) {
        CurrentTargetIndicator->SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y, 0.1f));
        CurrentTargetIndicator->SetActorRotation(FRotator(0.0f, 0.0f, 0.0f));
    }

    // Handle movement and ability execution when approaching target
    if(IsApproachingTarget && PendingAbilityTarget && PendingAbilityInputID != EAbilityInputID::None) {
        float Distance = FVector::Dist(GetActorLocation(), PendingAbilityTarget->GetActorLocation());

        int AbilitySlot = GetAbilitySlot(PendingAbilityInputID);
        if(UAbilityBase** FoundAbility = InstantiatedAbilities.Find(AbilitySlot)) {
            // Check if in range to execute ability
            if(Distance <= (*FoundAbility)->AbilityRange) {
                GetController()->StopMovement();

                ActiveAbilityInputID = PendingAbilityInputID;

                ConfirmAttack();

                // Clear pending target and flags
                PendingAbilityTarget = nullptr;
                PendingAbilityInputID = EAbilityInputID::None;
                IsApproachingTarget = false;
            }
        }
    }

    // Update mouse cursor state based on targeting or selection
    UpdateCursor();
}

// Helper to call a function by name with input action data
void ABaseCharacter::HandleFunctionCall(FName functionName, const FInputActionInstance& Instance) {
    if(UFunction* function = this->FindFunction(functionName)) {
        this->ProcessEvent(function, (void*)&Instance);
    }
}

// Called when the player starts setting a destination
void ABaseCharacter::OnSetDestinationStarted() {
    if(!CanMove) return;

    WasCancellingAbility = false;

    // If in ability targeting mode, cancel it
    if(IsInAbilityTargeting()) {
        CancelAttack();
        IsUsingAbility = false;
        WasCancellingAbility = true;
        return;
    }

    // If currently approaching an ability target, reset target and cancel attack
    if(IsApproachingTarget) {
        PendingAbilityTarget = nullptr;
        ClearEnemyTarget(PendingAbilityInputID);
        PendingAbilityInputID = EAbilityInputID::None;
        IsApproachingTarget = false;
        CancelAttack();
    }

    // Stop any current movement
    GetController()->StopMovement();
}

// Called while the player is holding the input to move
void ABaseCharacter::OnSetDestinationTriggered() {
    if(WasCancellingAbility || IsInAbilityTargeting() || !CanMove) return;

    FollowTime += GetWorld()->GetDeltaSeconds();

    APlayerController* PlayerController = Cast<APlayerController>(GetController());

    FHitResult Hit;
    bool bHitSuccessful = false;
    bHitSuccessful = PlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);

    if(bHitSuccessful) {
        CachedDestination = Hit.Location;
        CachedDestination.Z = GetActorLocation().Z; // Maintain character Z position
    }

    // Calculate direction and rotate character toward destination
    FVector WorldDirection = (CachedDestination - GetActorLocation()).GetSafeNormal();

    TargetRotation = WorldDirection.Rotation();
    ShouldRotate = true;

    // Move character in calculated direction
    AddMovementInput(WorldDirection, 1.0, false);
}

// Called when the player releases the movement input
void ABaseCharacter::OnSetDestinationReleased() {
    if(WasCancellingAbility) {
        WasCancellingAbility = false;
        return;
    }

    if(IsInAbilityTargeting() || !CanMove) return;

    // If short press, issue a move command to the clicked location
    if(FollowTime <= ShortPressThreshold) {
        APlayerController* PlayerController = Cast<APlayerController>(GetController());

        UAIBlueprintHelperLibrary::SimpleMoveToLocation(PlayerController, CachedDestination);
        // Spawn cursor effect at destination
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(PlayerController, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.0f, 1.0f, 1.0f), true, true, ENCPoolMethod::None, true);
    }

    FollowTime = 0.f;
}

// Activate ability by ability slot index
void ABaseCharacter::ActivateAbility(int AbilitySlot) {
    if(UAbilityBase** Ability = InstantiatedAbilities.Find(AbilitySlot)) {
        (*Ability)->ActivateAbility();
    }
}

// Ability input handlers calling ActivateAttackMode with corresponding input ID
void ABaseCharacter::Ability1() { ActivateAttackMode(EAbilityInputID::A); }
void ABaseCharacter::Ability2() { ActivateAttackMode(EAbilityInputID::Z); }
void ABaseCharacter::Ability3() { ActivateAttackMode(EAbilityInputID::E); }
void ABaseCharacter::Ability4() { ActivateAttackMode(EAbilityInputID::R); }

// Zoom camera in/out based on input axis value
void ABaseCharacter::ZoomCamera(const FInputActionInstance& Instance) {
    float axisValue = Instance.GetValue().Get<float>();

    if(FMath::Abs(axisValue) <= 0.3f) return; // Ignore small inputs

    float newLength = SpringArmComponent->TargetArmLength - axisValue * CameraZoomSpeed * GetWorld()->GetDeltaSeconds();
    newLength = FMath::Clamp(newLength, CameraZoomMin, CameraZoomMax);

    SpringArmComponent->TargetArmLength = newLength;
}

// Confirm and execute current active ability if possible
void ABaseCharacter::ConfirmAttack() {
    if(IsUsingAbility || ActiveAbilityInputID == EAbilityInputID::None || !CanUseAbility(ActiveAbilityInputID)) {
        CancelAttack();
        return;
    }

    IsUsingAbility = true;

    if(HUDWidget->AbilityClicked) {
        HUDWidget->AbilityClicked = false;
    }

    // Stop movement and execute ability
    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    PlayerController->StopMovement();
    ExecuteAbility(ActiveAbilityInputID);

    ActiveAbilityInputID = EAbilityInputID::None;
    OnAbilityOverlayHideRequested();
}

// Cancel current active ability input and hide overlay
void ABaseCharacter::CancelAttack() {
    if(ActiveAbilityInputID == EAbilityInputID::None) return;

    if(HUDWidget->AbilityClicked) {
        HUDWidget->AbilityClicked = false;
    }

    ActiveAbilityInputID = EAbilityInputID::None;

    OnAbilityOverlayHideRequested();
}

// Begin ability targeting mode if ability can be used
void ABaseCharacter::ActivateAttackMode(EAbilityInputID Ability) {
    if(IsUsingAbility || ActiveAbilityInputID != EAbilityInputID::None || !CanUseAbility(Ability)) {
        CancelAttack();
    }
    
    int AbilitySlot = GetAbilitySlot(Ability);

    // Check cooldown and resource cost
    if(UAbilityBase** FoundAbility = InstantiatedAbilities.Find(AbilitySlot)) {
        if((*FoundAbility)->IsOnCooldown || ((*FoundAbility)->RessourceCost > Ressource) && !(*FoundAbility)->CanRecast) {
            return;
        }
    }

    ActiveAbilityInputID = Ability;

    OnAbilityOverlayRequested(Ability);
}

// Spawn or update ability targeting indicator on screen
void ABaseCharacter::OnAbilityOverlayRequested(EAbilityInputID Ability) {
    OnAbilityOverlayHideRequested();

    if(const FAbiliyIndicatorSet* IndicatorSet = AbilityIndicators.Find(Ability)) {
        TSubclassOf<AAbilityTargetingIndicator> IndicatorToSpawn = nullptr;

        int AbilitySlot = GetAbilitySlot(Ability);

        if(!CanUseAbility(Ability)) return;

        if(UAbilityBase** FoundAbility = InstantiatedAbilities.Find(AbilitySlot)) {
            if((*FoundAbility)->Level == 0) return;

            if((*FoundAbility)->CanRecast) {
                IndicatorToSpawn = IndicatorSet->SecondCastIndicator;
            }else {
                IndicatorToSpawn = IndicatorSet->FirstCastIndicator;
            }
        }

        if(IndicatorToSpawn) {
            FActorSpawnParameters SpawnParameters;
            SpawnParameters.Owner = this;
            
            FVector SpawnLocation = GetActorLocation();
            SpawnLocation.Z = 0.1f;
            FRotator SpawnRotation = GetActorRotation();

            CurrentTargetIndicator = GetWorld()->SpawnActor<AAbilityTargetingIndicator>(IndicatorToSpawn, SpawnLocation, SpawnRotation, SpawnParameters);
        }else if(!HoverButton){
            ConfirmAttack();
        }
    }
}

// Destroy current targeting indicator if exists
void ABaseCharacter::OnAbilityOverlayHideRequested() {
    if(CurrentTargetIndicator) {
        CurrentTargetIndicator->Destroy();
        CurrentTargetIndicator = nullptr;
        HoverButton = false;
    }
}

void ABaseCharacter::ExecuteAbility(EAbilityInputID Ability) {
    // Get the player controller
    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    FHitResult HitLocation;
    FHitResult HitTarget;

    // Trace under cursor for two channels. Custom GameTraceChannel1 and Visibility
    if(PlayerController->GetHitResultUnderCursor(ECC_GameTraceChannel1, true, HitLocation) && PlayerController->GetHitResultUnderCursor(ECC_Visibility, true, HitTarget)) {
        // If this ability requires an enemy target and none is currently set
        if(NeedEnemyTarget(Ability) && !HasEnemyTarget(Ability)) {
            // Set the enemy target based on hit target actor
            SetEnemyTarget(Ability, HitTarget.GetActor());
            IsUsingAbility = false;
            // If target still not set, abort execution
            if(!HasEnemyTarget(Ability)) return;

            // Calculate distance to enemy target
            float Distance = FVector::Dist(GetActorLocation(), HitTarget.GetActor()->GetActorLocation());
            int AbilitySlot = GetAbilitySlot(Ability);

            // If target is out of range, move closer before executing ability
            if(UAbilityBase** FoundAbility = InstantiatedAbilities.Find(AbilitySlot)) {
                float AbilityRange = (*FoundAbility)->AbilityRange;

                if(Distance > AbilityRange) {
                    PendingAbilityTarget = HitTarget.GetActor();
                    PendingAbilityInputID = Ability;
                    IsApproachingTarget = true;
                    IsUsingAbility = false;

                    UAIBlueprintHelperLibrary::SimpleMoveToActor(PlayerController, PendingAbilityTarget);
                    return; // Wait to approach target before ability use
                }
            }
        }
        
        // Rotate character towards hit location on ground
        FVector TargetLocation = HitLocation.Location;
        FVector Direction = (TargetLocation - GetActorLocation()).GetSafeNormal();
        TargetRotation = Direction.Rotation();
        // Lock pitch and roll for rotation
        TargetRotation.Pitch = 0.0f;
        TargetRotation.Roll = 0.0f;
        ShouldRotate = true;
    }

    // Activate the ability now
    int AbilitySlot = GetAbilitySlot(Ability);
    ActivateAbility(AbilitySlot);
}

bool ABaseCharacter::IsInAbilityTargeting() const {
    // Check if any ability input is currently active
    return ActiveAbilityInputID != EAbilityInputID::None;
}

// Start a repeating timer to handle health/resource regeneration every second
void ABaseCharacter::LaunchRegen() {
    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ABaseCharacter::HandleRegen, 1.0f, true);
}

void ABaseCharacter::HandleRegen() {
    // Regenerate resource if below max
    if(Ressource < MaxRessource) {
        Ressource += RessourceRegen / 5.0f; // RessourceRegen is encoded for every 5 seconds so divide by 5 for each second
        Ressource = FMath::Clamp(Ressource, 0.0f, MaxRessource);
        HealthBarWidget->UpdateResourceOnChange(Ressource, MaxRessource);
        HUDWidget->UpdateResourceOnChange();
    }
    // Regenerate health if below max
    if(Health < MaxHealth) {
        Health += HealthRegen / 5.0f;
        Health = FMath::Clamp(Health, 0.0f, MaxHealth);
        HealthBarWidget->UpdateHealthOnChange(Health, MaxHealth);
        HUDWidget->UpdateHealthOnChange();
    }
}

bool ABaseCharacter::CanUseAbility(EAbilityInputID Ability) {
    // Check if ability can currently be used based on its CanBeUsed flag
    int AbilitySlot = GetAbilitySlot(Ability);
    if(UAbilityBase** FoundAbility = InstantiatedAbilities.Find(AbilitySlot)) {
        return (*FoundAbility)->CanBeUsed;
    }

    return false;
}

int ABaseCharacter::GetAbilitySlot(EAbilityInputID Ability) const {
    // Map ability input enum to ability slot index
    switch(Ability) {
        case EAbilityInputID::A:
            return 0;
        case EAbilityInputID::Z:
            return 1;
        case EAbilityInputID::E:
            return 2;
        case EAbilityInputID::R:
            return 3;
        default:
            return -1;
    }
}

void ABaseCharacter::LevelUP() {
    // Increase character level if not maxed, update stats and UI
    if(Level < MaxLevel) {
        Level++;
        Health += HealthPerLevel;
        Ressource += RessourcePerLevel;
        UpdateStats();
        HealthBarWidget->UpdateHealthOnChange(Health, MaxHealth);
        HealthBarWidget->UpdateResourceOnChange(Ressource, MaxRessource);
        HealthBarWidget->UpdateLevel();
    }
}

bool ABaseCharacter::NeedEnemyTarget(EAbilityInputID Ability) {
    // Determine if ability requires an enemy target, different for recast or not
    int AbilitySlot = GetAbilitySlot(Ability);
    if(UAbilityBase** FoundAbility = InstantiatedAbilities.Find(AbilitySlot)) {
        if(!(*FoundAbility)->CanRecast) {
            return (*FoundAbility)->NeedEnemyTarget;
        }
        if((*FoundAbility)->CanRecast) {
            return (*FoundAbility)->NeedEnemytargetRecast;
        }
    }

    return false;
}

void ABaseCharacter::SetEnemyTarget(EAbilityInputID Ability, AActor* Target) {
    // Assign an enemy target to the ability instance if valid
    int AbilitySlot = GetAbilitySlot(Ability);
    if(UAbilityBase** FoundAbility = InstantiatedAbilities.Find(AbilitySlot)) {
        if(ABaseCharacter* Character = Cast<ABaseCharacter>(Target)) {
            (*FoundAbility)->EnemyTarget = Character;
        }
    }
}

void ABaseCharacter::ClearEnemyTarget(EAbilityInputID Ability) {
    // Remove any assigned enemy target from the ability
    int AbilitySlot = GetAbilitySlot(Ability);
    if(UAbilityBase** FoundAbility = InstantiatedAbilities.Find(AbilitySlot)) {
        (*FoundAbility)->EnemyTarget = nullptr;
    }
}

bool ABaseCharacter::HasEnemyTarget(EAbilityInputID Ability) {
    // Check if ability currently has a valid enemy target assigned
    int AbilitySlot = GetAbilitySlot(Ability);
    if(UAbilityBase** FoundAbility = InstantiatedAbilities.Find(AbilitySlot)) {
        if((*FoundAbility)->EnemyTarget) return true;
    }

    return false;
}

void ABaseCharacter::UpdateCursor() {
    // Get custom player controller class for advanced cursor behavior
    APlayerController_Mechanics* PlayerController = Cast<APlayerController_Mechanics>(GetController());
    if(!PlayerController) return;
    
    if(IsInAbilityTargeting()) {
        int AbilitySlot = GetAbilitySlot(ActiveAbilityInputID);
        if(UAbilityBase** FoundAbility = InstantiatedAbilities.Find(AbilitySlot)) {
            FHitResult Hit;
            PlayerController->GetHitResultUnderCursor(ECC_Visibility, true, Hit);

            // If hovering over an enemy and ability requires target, show enemy targeting cursor
            if(Hit.bBlockingHit && Cast<ABaseCharacter>(Hit.GetActor()) && (*FoundAbility)->NeedEnemyTarget) {
                PlayerController->SetMouseCursorWidget(EMouseCursor::Default, PlayerController->TargetingEnemyCursorBrush);
                // return;
            }else {
                // Otherwise, show normal targeting cursor
                PlayerController->SetMouseCursorWidget(EMouseCursor::Default, PlayerController->TargetingCursorBrush);
            }
        }
    }else {
        // If not targeting ability, use default cursor
        PlayerController->SetMouseCursorWidget(EMouseCursor::Default, PlayerController->DefaultCursorBrush);
    }

    // Trick to force mouse cursor redraw/update when switching cursor to avoid invisible on state change until player moves it
    FVector2D CurrentMousePosition;
    PlayerController->GetMousePosition(CurrentMousePosition.X, CurrentMousePosition.Y);
    PlayerController->SetMouseLocation(CurrentMousePosition.X + 1, CurrentMousePosition.Y);
    PlayerController->SetMouseLocation(CurrentMousePosition.X, CurrentMousePosition.Y);
}
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
    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArmComponent->SetupAttachment(RootComponent);
    SpringArmComponent->TargetArmLength = 2000.f;
    SpringArmComponent->bDoCollisionTest = false;
    SpringArmComponent->bUsePawnControlRotation = false;
    SpringArmComponent->bInheritPitch = false;
    SpringArmComponent->bInheritRoll = false;
    SpringArmComponent->bInheritYaw = false;

    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
    CameraComponent->FieldOfView = 60.0f;
    CameraComponent->bUsePawnControlRotation = false;

    GetMesh()->SetConstraintMode(EDOFMode::SixDOF);
    GetMesh()->BodyInstance.bLockXRotation = true;
    GetMesh()->BodyInstance.bLockYRotation = true;

    Level = 1;

    CachedDestination = FVector::ZeroVector;
    FollowTime = 0.0f;
}

void ABaseCharacter::BeginPlay() {
	Super::BeginPlay();

    UWidgetComponent* WidgetComponent = Cast<UWidgetComponent>(GetComponentByClass(UWidgetComponent::StaticClass()));

    if(WidgetComponent) {
        HealthBarWidget = Cast<UHealthBar>(WidgetComponent->GetUserWidgetObject());
    }

    bUseControllerRotationYaw = false;
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;

    SpringArmComponent->SetRelativeRotation(FRotator(-45.0f, 0.f, 0.f));

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

    if(ShouldRotate) {
        FRotator CurrentRotation = GetActorRotation();
        FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, RotationSpeed);
        SetActorRotation(NewRotation);

        if(NewRotation.Equals(TargetRotation, 1.0f)) {
            ShouldRotate = false;
        }
    }

    if(CurrentTargetIndicator) {
        CurrentTargetIndicator->SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y, 0.1f));
        CurrentTargetIndicator->SetActorRotation(FRotator(0.0f, 0.0f, 0.0f));
    }

    if(IsApproachingTarget && PendingAbilityTarget && PendingAbilityInputID != EAbilityInputID::None) {
        float Distance = FVector::Dist(GetActorLocation(), PendingAbilityTarget->GetActorLocation());

        int AbilitySlot = GetAbilitySlot(PendingAbilityInputID);
        if(UAbilityBase** FoundAbility = InstantiatedAbilities.Find(AbilitySlot)) {
            if(Distance <= (*FoundAbility)->AbilityRange) {
                GetController()->StopMovement();

                ActiveAbilityInputID = PendingAbilityInputID;

                ConfirmAttack();

                PendingAbilityTarget = nullptr;
                PendingAbilityInputID = EAbilityInputID::None;
                IsApproachingTarget = false;
            }
        }
    }

    UpdateCursor();
}

void ABaseCharacter::HandleFunctionCall(FName functionName, const FInputActionInstance& Instance) {
    if(UFunction* function = this->FindFunction(functionName)) {
        this->ProcessEvent(function, (void*)&Instance);
    }
}

void ABaseCharacter::OnSetDestinationStarted() {
    if(!CanMove) return;

    WasCancellingAbility = false;

    if(IsInAbilityTargeting()) {
        CancelAttack();
        IsUsingAbility = false;
        WasCancellingAbility = true;
        return;
    }

    if(IsApproachingTarget) {
        PendingAbilityTarget = nullptr;
        ClearEnemyTarget(PendingAbilityInputID);
        PendingAbilityInputID = EAbilityInputID::None;
        IsApproachingTarget = false;
        CancelAttack();
    }

    GetController()->StopMovement();
}

void ABaseCharacter::OnSetDestinationTriggered() {
    if(WasCancellingAbility || IsInAbilityTargeting() || !CanMove) return;

    FollowTime += GetWorld()->GetDeltaSeconds();

    APlayerController* PlayerController = Cast<APlayerController>(GetController());

    FHitResult Hit;
    bool bHitSuccessful = false;
    bHitSuccessful = PlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);

    if(bHitSuccessful) {
        CachedDestination = Hit.Location;
        CachedDestination.Z = GetActorLocation().Z;
    }

    FVector WorldDirection = (CachedDestination - GetActorLocation()).GetSafeNormal();

    TargetRotation = WorldDirection.Rotation();
    ShouldRotate = true;

    AddMovementInput(WorldDirection, 1.0, false);
}

void ABaseCharacter::OnSetDestinationReleased() {
    if(WasCancellingAbility) {
        WasCancellingAbility = false;
        return;
    }

    if(IsInAbilityTargeting() || !CanMove) return;

    if(FollowTime <= ShortPressThreshold) {
        APlayerController* PlayerController = Cast<APlayerController>(GetController());

        UAIBlueprintHelperLibrary::SimpleMoveToLocation(PlayerController, CachedDestination);
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(PlayerController, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.0f, 1.0f, 1.0f), true, true, ENCPoolMethod::None, true);
    }

    FollowTime = 0.f;
}

void ABaseCharacter::ActivateAbility(int AbilitySlot) {
    if(UAbilityBase** Ability = InstantiatedAbilities.Find(AbilitySlot)) {
        (*Ability)->ActivateAbility();
    }
}

void ABaseCharacter::Ability1() {
    ActivateAttackMode(EAbilityInputID::A);
}

void ABaseCharacter::Ability2() {
    ActivateAttackMode(EAbilityInputID::Z);
}

void ABaseCharacter::Ability3() {
    ActivateAttackMode(EAbilityInputID::E);
}

void ABaseCharacter::Ability4() {    
    ActivateAttackMode(EAbilityInputID::R);
}

void ABaseCharacter::ZoomCamera(const FInputActionInstance& Instance) {
    float axisValue = Instance.GetValue().Get<float>();

    if(FMath::Abs(axisValue) <= 0.3f) return;

    float newLength = SpringArmComponent->TargetArmLength - axisValue * CameraZoomSpeed * GetWorld()->GetDeltaSeconds();
    newLength = FMath::Clamp(newLength, CameraZoomMin, CameraZoomMax);

    SpringArmComponent->TargetArmLength = newLength;
}

void ABaseCharacter::ConfirmAttack() {
    if(IsUsingAbility || ActiveAbilityInputID == EAbilityInputID::None || !CanUseAbility(ActiveAbilityInputID)) {
        CancelAttack();
        return;
    }

    IsUsingAbility = true;

    if(HUDWidget->AbilityClicked) {
        HUDWidget->AbilityClicked = false;
    }

    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    PlayerController->StopMovement();
    ExecuteAbility(ActiveAbilityInputID);

    ActiveAbilityInputID = EAbilityInputID::None;
    OnAbilityOverlayHideRequested();
}

void ABaseCharacter::CancelAttack() {
    if(ActiveAbilityInputID == EAbilityInputID::None) return;

    if(HUDWidget->AbilityClicked) {
        HUDWidget->AbilityClicked = false;
    }

    ActiveAbilityInputID = EAbilityInputID::None;

    OnAbilityOverlayHideRequested();
}

void ABaseCharacter::ActivateAttackMode(EAbilityInputID Ability) {
    if(IsUsingAbility || ActiveAbilityInputID != EAbilityInputID::None || !CanUseAbility(Ability)) {
        CancelAttack();
    }
    
    int AbilitySlot = GetAbilitySlot(Ability);

    if(UAbilityBase** FoundAbility = InstantiatedAbilities.Find(AbilitySlot)) {
        if((*FoundAbility)->IsOnCooldown || ((*FoundAbility)->RessourceCost > Ressource) && !(*FoundAbility)->CanRecast) {
            return;
        }
    }

    ActiveAbilityInputID = Ability;

    OnAbilityOverlayRequested(Ability);
}

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

void ABaseCharacter::OnAbilityOverlayHideRequested() {
    if(CurrentTargetIndicator) {
        CurrentTargetIndicator->Destroy();
        CurrentTargetIndicator = nullptr;
        HoverButton = false;
    }
}

void ABaseCharacter::ExecuteAbility(EAbilityInputID Ability) {
    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    FHitResult HitLocation;
    FHitResult HitTarget;

    if(PlayerController->GetHitResultUnderCursor(ECC_GameTraceChannel1, true, HitLocation) && PlayerController->GetHitResultUnderCursor(ECC_Visibility, true, HitTarget)) {
        if(NeedEnemyTarget(Ability) && !HasEnemyTarget(Ability)) {
            SetEnemyTarget(Ability, HitTarget.GetActor());
            IsUsingAbility = false;
            if(!HasEnemyTarget(Ability)) return;

            float Distance = FVector::Dist(GetActorLocation(), HitTarget.GetActor()->GetActorLocation());
            int AbilitySlot = GetAbilitySlot(Ability);

            if(UAbilityBase** FoundAbility = InstantiatedAbilities.Find(AbilitySlot)) {
                float AbilityRange = (*FoundAbility)->AbilityRange;

                if(Distance > AbilityRange) {
                    PendingAbilityTarget = HitTarget.GetActor();
                    PendingAbilityInputID = Ability;
                    IsApproachingTarget = true;
                    IsUsingAbility = false;

                    UAIBlueprintHelperLibrary::SimpleMoveToActor(PlayerController, PendingAbilityTarget);
                    return;
                }
            }
        }
        
        FVector TargetLocation = HitLocation.Location;
        FVector Direction = (TargetLocation - GetActorLocation()).GetSafeNormal();
        TargetRotation = Direction.Rotation();
        TargetRotation.Pitch = 0.0f;
        TargetRotation.Roll = 0.0f;
        ShouldRotate = true;
    }

    int AbilitySlot = GetAbilitySlot(Ability);
    ActivateAbility(AbilitySlot);
}

bool ABaseCharacter::IsInAbilityTargeting() const {
    return ActiveAbilityInputID != EAbilityInputID::None;
}

void ABaseCharacter::LaunchRegen() {
    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ABaseCharacter::HandleRegen, 1.0f, true);
}

void ABaseCharacter::HandleRegen() {
    if(Ressource < MaxRessource) {
        Ressource += RessourceRegen / 5.0f;
        Ressource = FMath::Clamp(Ressource, 0.0f, MaxRessource);
        HealthBarWidget->UpdateResourceOnChange(Ressource, MaxRessource);
        HUDWidget->UpdateResourceOnChange();
    }
    if(Health < MaxHealth) {
        Health += HealthRegen / 5.0f;
        Health = FMath::Clamp(Health, 0.0f, MaxHealth);
        HealthBarWidget->UpdateHealthOnChange(Health, MaxHealth);
        HUDWidget->UpdateHealthOnChange();
    }
}

bool ABaseCharacter::CanUseAbility(EAbilityInputID Ability) {
    int AbilitySlot = GetAbilitySlot(Ability);
    if(UAbilityBase** FoundAbility = InstantiatedAbilities.Find(AbilitySlot)) {
        return (*FoundAbility)->CanBeUsed;
    }

    return false;
}

int ABaseCharacter::GetAbilitySlot(EAbilityInputID Ability) const {
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
    int AbilitySlot = GetAbilitySlot(Ability);
    if(UAbilityBase** FoundAbility = InstantiatedAbilities.Find(AbilitySlot)) {
        if(ABaseCharacter* Character = Cast<ABaseCharacter>(Target)) {
            (*FoundAbility)->EnemyTarget = Character;
        }
    }
}

void ABaseCharacter::ClearEnemyTarget(EAbilityInputID Ability) {
    int AbilitySlot = GetAbilitySlot(Ability);
    if(UAbilityBase** FoundAbility = InstantiatedAbilities.Find(AbilitySlot)) {
        (*FoundAbility)->EnemyTarget = nullptr;
    }
}

bool ABaseCharacter::HasEnemyTarget(EAbilityInputID Ability) {
    int AbilitySlot = GetAbilitySlot(Ability);
    if(UAbilityBase** FoundAbility = InstantiatedAbilities.Find(AbilitySlot)) {
        if((*FoundAbility)->EnemyTarget) return true;
    }

    return false;
}

void ABaseCharacter::UpdateCursor() {
    APlayerController_Mechanics* PlayerController = Cast<APlayerController_Mechanics>(GetController());
    if(!PlayerController) return;
    
    if(IsInAbilityTargeting()) {
        int AbilitySlot = GetAbilitySlot(ActiveAbilityInputID);
        if(UAbilityBase** FoundAbility = InstantiatedAbilities.Find(AbilitySlot)) {
            FHitResult Hit;
            PlayerController->GetHitResultUnderCursor(ECC_Visibility, true, Hit);

            if(Hit.bBlockingHit && Cast<ABaseCharacter>(Hit.GetActor()) && (*FoundAbility)->NeedEnemyTarget) {
                PlayerController->SetMouseCursorWidget(EMouseCursor::Default, PlayerController->TargetingEnemyCursorBrush);
                // return;
            }else {
                PlayerController->SetMouseCursorWidget(EMouseCursor::Default, PlayerController->TargetingCursorBrush);
            }
        }
    }else {
        PlayerController->SetMouseCursorWidget(EMouseCursor::Default, PlayerController->DefaultCursorBrush);
    }

    FVector2D CurrentMousePosition;
    PlayerController->GetMousePosition(CurrentMousePosition.X, CurrentMousePosition.Y);
    PlayerController->SetMouseLocation(CurrentMousePosition.X + 1, CurrentMousePosition.Y);
    PlayerController->SetMouseLocation(CurrentMousePosition.X, CurrentMousePosition.Y);
}
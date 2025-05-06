#include "BaseCharacter.h"
#include "../Ability/AbilityBase.h"
#include "InputAction.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "../Ability/AbilityTargetingIndicator.h"
#include "GameFramework/CharacterMovementComponent.h"

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
        CurrentTargetIndicator->SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y, 0.0f));
        CurrentTargetIndicator->SetActorRotation(FRotator(0.0f, 0.0f, 0.0f));
    }
}

void ABaseCharacter::HandleFunctionCall(FName functionName, const FInputActionInstance& Instance) {
    if(UFunction* function = this->FindFunction(functionName)) {
        this->ProcessEvent(function, (void*)&Instance);
    }
}

void ABaseCharacter::OnSetDestinationStarted() {
    WasCancellingAbility = false;

    if(IsInAbilityTargeting()) {
        CancelAttack();
        WasCancellingAbility = true;
        return;
    }

    GetController()->StopMovement();
}

void ABaseCharacter::OnSetDestinationTriggered() {
    if(WasCancellingAbility || IsInAbilityTargeting()) return;

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

    if(IsInAbilityTargeting()) return;

    if(FollowTime <= ShortPressThreshold) {
        APlayerController* PlayerController = Cast<APlayerController>(GetController());

        UAIBlueprintHelperLibrary::SimpleMoveToLocation(PlayerController, CachedDestination);
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(PlayerController, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.0f, 1.0f, 1.0f), true, true, ENCPoolMethod::None, true);
    }

    FollowTime = 0.f;
}

void ABaseCharacter::ActivateAbility(FName AbilitySlot) {
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
    if(ActiveAbilityInputID == EAbilityInputID::None) return;

    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    PlayerController->StopMovement();

    ExecuteAbility(ActiveAbilityInputID);

    ActiveAbilityInputID = EAbilityInputID::None;
    OnAbilityOverlayHideRequested();
}

void ABaseCharacter::CancelAttack() {
    if(ActiveAbilityInputID == EAbilityInputID::None) return;

    ActiveAbilityInputID = EAbilityInputID::None;
    OnAbilityOverlayHideRequested();
}

void ABaseCharacter::ActivateAttackMode(EAbilityInputID Ability) {
    if(ActiveAbilityInputID != EAbilityInputID::None) {
        CancelAttack();
    }

    FString AbilityName = UEnum::GetValueAsString(Ability);
    AbilityName = AbilityName.RightChop(AbilityName.Find(TEXT("::")) + 2);
    if(UAbilityBase** FoundAbility = InstantiatedAbilities.Find(*AbilityName)) {
        if(*FoundAbility && (*FoundAbility)->IsOnCooldown) {
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

        FString AbilityName = UEnum::GetValueAsString(Ability);
        AbilityName = AbilityName.RightChop(AbilityName.Find(TEXT("::")) + 2);

        if(UAbilityBase** FoundAbility = InstantiatedAbilities.Find(*AbilityName)) {
            if(!*FoundAbility || (*FoundAbility)->Level == 0) return;

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
            SpawnLocation.Z = 0.0f;
            FRotator SpawnRotation = GetActorRotation();

            CurrentTargetIndicator = GetWorld()->SpawnActor<AAbilityTargetingIndicator>(IndicatorToSpawn, SpawnLocation, SpawnRotation, SpawnParameters);
        }else {
            ConfirmAttack();
        }
    }
}

void ABaseCharacter::OnAbilityOverlayHideRequested() {
    if(CurrentTargetIndicator) {
        CurrentTargetIndicator->Destroy();
        CurrentTargetIndicator = nullptr;
    }
}

void ABaseCharacter::ExecuteAbility(EAbilityInputID Ability) {
    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    FHitResult Hit;

    if(PlayerController->GetHitResultUnderCursor(ECC_Visibility, true, Hit)) {
        FVector TargetLocation = Hit.Location;
        FVector Direction = (TargetLocation - GetActorLocation()).GetSafeNormal();
        TargetRotation = Direction.Rotation();
        ShouldRotate = true;
    }

    switch(Ability) {
        case EAbilityInputID::A:
            ActivateAbility("A");
            break;
        case EAbilityInputID::Z:
            ActivateAbility("Z");
            break;
        case EAbilityInputID::E:
            ActivateAbility("E");
            break;
        case EAbilityInputID::R:
            ActivateAbility("R");
            break;
        default:
            break;
    }
}

bool ABaseCharacter::IsInAbilityTargeting() const {
    return ActiveAbilityInputID != EAbilityInputID::None;
}
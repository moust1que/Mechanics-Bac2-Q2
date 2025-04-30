#include "BaseCharacter.h"
#include "../Ability/AbilityBase.h"
#include "InputAction.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

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

    // Stats
    Level = 1;
}

void ABaseCharacter::BeginPlay() {
	Super::BeginPlay();

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

void ABaseCharacter::HandleFunctionCall(FName functionName, const FInputActionInstance& Instance) {
    if(UFunction* function = this->FindFunction(functionName)) {
        this->ProcessEvent(function, (void*)&Instance);
    }
}

void ABaseCharacter::ActivateAbility(FName AbilitySlot) {
    if(UAbilityBase** Ability = InstantiatedAbilities.Find(AbilitySlot)) {
        (*Ability)->ActivateAbility();
    }
}

void ABaseCharacter::Ability1() {
    ActivateAbility("A");
}

void ABaseCharacter::Ability2() {
    ActivateAbility("Z");
}

void ABaseCharacter::Ability3() {
    ActivateAbility("E");
}

void ABaseCharacter::Ability4() {    
    ActivateAbility("R");
}

void ABaseCharacter::ZoomCamera(const FInputActionInstance& Instance) {
    float axisValue = Instance.GetValue().Get<float>();

    if(FMath::Abs(axisValue) <= 0.3f) return;

    float newLength = SpringArmComponent->TargetArmLength - axisValue * CameraZoomSpeed * GetWorld()->GetDeltaSeconds();
    newLength = FMath::Clamp(newLength, CameraZoomMin, CameraZoomMax);

    SpringArmComponent->TargetArmLength = newLength;
}
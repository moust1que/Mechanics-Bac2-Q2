#include "BaseCharacter.h"
#include "../Ability/AbilityBase.h"
#include "InputAction.h"

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay() {
	Super::BeginPlay();

    for(const auto& AbilityPair : AbilityMap) {
        if(AbilityPair.Value) {
            UAbilityBase* InstantiatedAbility = NewObject<UAbilityBase>(this, AbilityPair.Value);
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
        (*Ability)->ActivateAbility(this);
    }
}
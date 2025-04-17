#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AbilityBase.generated.h"

// Forward declaration
class ACharacter;

UCLASS()
class MECHANICS_API UAbilityBase : public UObject {
	GENERATED_BODY()

    public:
        UFUNCTION() void ActivateAbility(ACharacter* OwnerCharacter) {};
};
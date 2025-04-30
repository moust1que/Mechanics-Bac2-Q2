#pragma once

#include "CoreMinimal.h"
#include "../../AbilityBase.h"
#include "PerfectExecution.generated.h"

UCLASS()
class MECHANICS_API UPerfectExecution : public UAbilityBase {
	GENERATED_BODY()

    public:
        UFUNCTION() void ActivateAbility() override;
};
#pragma once

#include "CoreMinimal.h"
#include "../../AbilityTargetingIndicator.h"
#include "PerfectExecutionIndicator.generated.h"

UCLASS()
class MECHANICS_API APerfectExecutionIndicator : public AAbilityTargetingIndicator {
	GENERATED_BODY()

    public:
        APerfectExecutionIndicator();

        UFUNCTION() void SetAttackRange(float Range) override;

    protected:
        // The decal component used to visually represent the ability's range on the ground
        UPROPERTY(VisibleAnywhere) UDecalComponent* RangeDecal;
};
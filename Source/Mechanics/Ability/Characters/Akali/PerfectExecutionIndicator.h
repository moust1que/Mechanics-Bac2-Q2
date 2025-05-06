#pragma once

#include "CoreMinimal.h"
#include "../../AbilityTargetingIndicator.h"
#include "PerfectExecutionIndicator.generated.h"

// Forward Declaration

UCLASS()
class MECHANICS_API APerfectExecutionIndicator : public AAbilityTargetingIndicator {
	GENERATED_BODY()

    public:
        APerfectExecutionIndicator();

        UFUNCTION() void UpdateIndicatorDirection(const FVector& Direction) override;
        UFUNCTION() void SetAttackRange(float Range) override;

    protected:
        UPROPERTY(VisibleAnywhere) UDecalComponent* RangeDecal;
};
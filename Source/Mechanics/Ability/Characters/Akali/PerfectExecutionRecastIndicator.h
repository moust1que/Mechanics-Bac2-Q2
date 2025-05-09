#pragma once

#include "CoreMinimal.h"
#include "../../AbilityTargetingIndicator.h"
#include "PerfectExecutionRecastIndicator.generated.h"

UCLASS()
class MECHANICS_API APerfectExecutionRecastIndicator : public AAbilityTargetingIndicator {
	GENERATED_BODY()

    public:
        APerfectExecutionRecastIndicator();

        UFUNCTION() void UpdateIndicatorDirection(const FVector& Direction) override;
        UFUNCTION() void SetAttackRange(float Range) override;

    protected:
        UPROPERTY(VisibleAnywhere) UStaticMeshComponent* ArrowTipMesh;
        UPROPERTY(VisibleAnywhere) UStaticMeshComponent* ArrowBaseMesh;
};
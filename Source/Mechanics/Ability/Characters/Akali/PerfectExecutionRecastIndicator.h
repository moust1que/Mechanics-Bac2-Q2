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
        // Mesh component representing the tip of the arrow
        UPROPERTY(VisibleAnywhere) UStaticMeshComponent* ArrowTipMesh;
        // Mesh component representing the base of the arrow
        UPROPERTY(VisibleAnywhere) UStaticMeshComponent* ArrowBaseMesh;
};
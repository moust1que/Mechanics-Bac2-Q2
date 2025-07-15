#pragma once

#include "CoreMinimal.h"
#include "../../AbilityTargetingIndicator.h"
#include "RocketGrabIndicator.generated.h"

UCLASS()
class MECHANICS_API ARocketGrabIndicator : public AAbilityTargetingIndicator {
	GENERATED_BODY()

    public:
        ARocketGrabIndicator();

        UFUNCTION() void UpdateIndicatorDirection(const FVector& Direction) override;
        UFUNCTION() void SetAttackRange(float Range) override;

    protected:
        // Mesh representing the tip of the targeting arrow
        UPROPERTY(VisibleAnywhere) UStaticMeshComponent* ArrowTipMesh;
        // Mesh representing the base of the targeting arrow
        UPROPERTY(VisibleAnywhere) UStaticMeshComponent* ArrowBaseMesh;
};
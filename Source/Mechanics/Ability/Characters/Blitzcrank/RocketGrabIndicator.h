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
        UPROPERTY(VisibleAnywhere) UStaticMeshComponent* ArrowTipMesh;
        UPROPERTY(VisibleAnywhere) UStaticMeshComponent* ArrowBaseMesh;
};
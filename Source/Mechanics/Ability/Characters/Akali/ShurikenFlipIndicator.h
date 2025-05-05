#pragma once

#include "CoreMinimal.h"
#include "../../AbilityTargetingIndicator.h"
#include "ShurikenFlipIndicator.generated.h"

// Forward declaration
class UDecalComponent;
class UStaticMeshComponent;

UCLASS()
class MECHANICS_API AShurikenFlipIndicator : public AAbilityTargetingIndicator {
	GENERATED_BODY()

    public:
        AShurikenFlipIndicator();
        
        UFUNCTION() void UpdateIndicatorDirection(const FVector& Direction) override;
        UFUNCTION() void SetAttackRange(float Range) override;

    protected:
        UPROPERTY(VisibleAnywhere) UDecalComponent* RangeDecal;
        UPROPERTY(VisibleAnywhere) UStaticMeshComponent* BackArrowMesh;
        UPROPERTY(VisibleAnywhere) UStaticMeshComponent* FrontArrowMesh;
        UPROPERTY(VisibleAnywhere) UStaticMeshComponent* FrontArrowTipMesh;
};
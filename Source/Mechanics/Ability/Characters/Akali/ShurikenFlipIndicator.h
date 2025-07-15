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
    // Visual component displaying the ground range decal of the ability
        UPROPERTY(VisibleAnywhere) UDecalComponent* RangeDecal;
        // Mesh components representing the back arrow and front arrow parts of the indicator
        UPROPERTY(VisibleAnywhere) UStaticMeshComponent* BackArrowMesh;
        UPROPERTY(VisibleAnywhere) UStaticMeshComponent* FrontArrowMesh;
        UPROPERTY(VisibleAnywhere) UStaticMeshComponent* FrontArrowTipMesh;
};
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShurikenFlipTargetingIndicator.generated.h"

// Forward declaration
class UDecalComponent;
class UStaticMeshComponent;

UCLASS()
class MECHANICS_API AShurikenFlipTargetingIndicator : public AActor {
	GENERATED_BODY()
	
    public:
        AShurikenFlipTargetingIndicator();

        // virtual void Tick(float DeltaTime) override;

        void UpdateIndicatorDirection(const FVector& Direction);

        void SetAttackRange(float Range);

        void ShowIndicator(bool Show);

    protected:
        // virtual void BeginPlay() override;

        float AttackRange = 600.0f;

        UPROPERTY(VisibleAnywhere) UDecalComponent* RangeDecal;

        UPROPERTY(VisibleAnywhere) UStaticMeshComponent* BackArrowMesh;

        UPROPERTY(VisibleAnywhere) UStaticMeshComponent* FrontArrowMesh;

        UPROPERTY(VisibleAnywhere) UStaticMeshComponent* FrontArrowTipMesh;
};
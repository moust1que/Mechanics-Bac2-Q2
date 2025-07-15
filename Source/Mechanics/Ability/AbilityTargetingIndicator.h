#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilityTargetingIndicator.generated.h"

UCLASS()
class MECHANICS_API AAbilityTargetingIndicator : public AActor {
	GENERATED_BODY()
	
    public:
        virtual void UpdateIndicatorDirection(const FVector& Direction) {};
        virtual void SetAttackRange(float Range) {};
        void ShowIndicator(bool Show);

    protected:
        // Distance in Unreal units that the indicator should represent by default
        float AttackRange = 600.0f;
};
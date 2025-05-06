#include "PerfectExecutionIndicator.h"
#include "Components/DecalComponent.h"

APerfectExecutionIndicator::APerfectExecutionIndicator() {
    PrimaryActorTick.bCanEverTick = false;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    RangeDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("RangeDecal"));
    RangeDecal->SetupAttachment(RootComponent);
    RangeDecal->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
}

void APerfectExecutionIndicator::UpdateIndicatorDirection(const FVector& Direction) {
    SetAttackRange(675.0f);
}

void APerfectExecutionIndicator::SetAttackRange(float Range) {
    AttackRange = Range;
    RangeDecal->SetRelativeScale3D(FVector(1.0f, AttackRange, AttackRange));
    RangeDecal->DecalSize = FVector(1.0f, 1.0f, 1.0f);
}
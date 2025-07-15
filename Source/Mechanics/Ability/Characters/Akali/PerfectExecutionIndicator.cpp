#include "PerfectExecutionIndicator.h"
#include "Components/DecalComponent.h"

// Constructor: sets up the indicator's components and default configuration
APerfectExecutionIndicator::APerfectExecutionIndicator() {
    // Disable ticking since this indicator does not need updates every frame
    PrimaryActorTick.bCanEverTick = false;

    // Create the root component for the actor
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    // Create and attach a DecalComponent to visualize the attack range
    RangeDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("RangeDecal"));
    RangeDecal->SetupAttachment(RootComponent);
    // Rotate the decal to project flat onto the ground
    RangeDecal->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));

    // Simply sets a fixed attack range; could be expanded to orient toward target
    SetAttackRange(675.0f);
}

// Sets the visible range of the decal based on the ability's attack range
void APerfectExecutionIndicator::SetAttackRange(float Range) {
    AttackRange = Range;
    // Adjust the decal’s scale to reflect the attack range
    RangeDecal->SetRelativeScale3D(FVector(1.0f, AttackRange, AttackRange));
    // Sets the actual decal size; here it's static and doesn't change with range
    RangeDecal->DecalSize = FVector(1.0f, 1.0f, 1.0f);
}
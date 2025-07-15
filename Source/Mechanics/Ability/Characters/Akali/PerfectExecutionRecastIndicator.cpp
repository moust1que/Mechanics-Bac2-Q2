#include "PerfectExecutionRecastIndicator.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Constructor: sets up components and enables ticking
APerfectExecutionRecastIndicator::APerfectExecutionRecastIndicator() {
    // Enable Tick() as this indicator may be updated dynamically every frame
    PrimaryActorTick.bCanEverTick = true;
    
    // Create and assign root component
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    
    // Create the base mesh of the arrow
    ArrowBaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArrowBaseMesh"));
    ArrowBaseMesh->SetupAttachment(RootComponent);
    // Rotate it so the arrow lies flat on the ground
    ArrowBaseMesh->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
    
    // Create the tip mesh of the arrow
    ArrowTipMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArrowTipMesh"));
    ArrowTipMesh->SetupAttachment(RootComponent);
    // Same rotation to match the base
    ArrowTipMesh->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
}

// Called to update the direction and placement of the recast indicator
void APerfectExecutionRecastIndicator::UpdateIndicatorDirection(const FVector& Direction) {
    // Set the fixed recast range (Akali dashes 800 units on recast)
    SetAttackRange(800.0f);

    // Normalize the input direction to ensure it's unit length
    FVector Forward = Direction.GetSafeNormal();

    // Create a rotation from the direction vector
    FRotator LookAtRotation = UKismetMathLibrary::MakeRotFromX(Forward);

    // Offset to rotate the arrow mesh correctly
    FRotator RotationOffset(0.0f, 90.0f, 0.0f);

    // Offset the arrow base and tip slightly so they are visually centered
    float RecenterBaseOffset = 41.0f;
    FVector BaseOffset = Forward * RecenterBaseOffset;
    float RecenterTipOffset = 75.0f;
    FVector TipOffset = Forward * RecenterTipOffset;

    // Calculate final positions of the arrow base and tip
    FVector BaseLocation = Forward * (AttackRange * 0.5f) - BaseOffset;
    FVector TipLocation = Forward * AttackRange - TipOffset;

    // Compute distance between base and tip
    float Distance = FVector::Dist(BaseLocation, TipLocation);

    // Set transform for the base mesh
    ArrowBaseMesh->SetRelativeScale3D(FVector(1.1f, 6.35f, 1.0f));
    ArrowBaseMesh->SetRelativeRotation(LookAtRotation + RotationOffset);
    ArrowBaseMesh->SetRelativeLocation(BaseLocation);

    // Set transform for the tip mesh
    ArrowTipMesh->SetRelativeScale3D(FVector(1.1f, 1.0f, 1.0f));
    ArrowTipMesh->SetRelativeRotation(LookAtRotation + RotationOffset);
    ArrowTipMesh->SetRelativeLocation(TipLocation);
}

// Sets the attack range for the recast indicator
void APerfectExecutionRecastIndicator::SetAttackRange(float Range) {
    AttackRange = Range;
}
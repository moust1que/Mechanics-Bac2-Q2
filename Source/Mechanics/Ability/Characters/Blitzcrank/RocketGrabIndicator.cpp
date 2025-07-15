#include "RocketGrabIndicator.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"

ARocketGrabIndicator::ARocketGrabIndicator() {
    PrimaryActorTick.bCanEverTick = true;
    
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    
    // Create the base arrow mesh component and attach it to the root
    ArrowBaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArrowBaseMesh"));
    ArrowBaseMesh->SetupAttachment(RootComponent);
    ArrowBaseMesh->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
    
    // Create the arrow tip mesh component and attach it to the root
    ArrowTipMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArrowTipMesh"));
    ArrowTipMesh->SetupAttachment(RootComponent);
    ArrowTipMesh->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
}

// Update the direction and position of the indicator arrow based on the given direction vector
void ARocketGrabIndicator::UpdateIndicatorDirection(const FVector& Direction) {
    // Set the effective attack range for the indicator
    SetAttackRange(1115.0f);
    
    // Normalize the input direction vector to get a unit forward vector
    FVector Forward = Direction.GetSafeNormal();

    // Calculate the rotation that points along the forward vector
    FRotator LookAtRotation = UKismetMathLibrary::MakeRotFromX(Forward);

    // Rotation offset to adjust the meshes' orientation
    FRotator RotationOffset(0.0f, 90.0f, 0.0f);

    // Offsets to recenter the base and tip meshes along the forward vector
    float RecenterBaseOffset = 41.0f;
    FVector BaseOffset = Forward * RecenterBaseOffset;
    float RecenterTipOffset = 75.0f;
    FVector TipOffset = Forward * RecenterTipOffset;

    // Calculate the relative location for the base mesh
    FVector BaseLocation = Forward * (AttackRange * 0.5f) - BaseOffset;
    // Calculate the relative location for the tip mesh
    FVector TipLocation = Forward * AttackRange - TipOffset;

    // Scale the base mesh to stretch it appropriately along the Y-axis
    ArrowBaseMesh->SetRelativeScale3D(FVector(1.4f, 9.48f, 1.0f));
    // Rotate the base mesh with the calculated look rotation plus offset
    ArrowBaseMesh->SetRelativeRotation(LookAtRotation + RotationOffset);
    // Set the base mesh to its calculated position
    ArrowBaseMesh->SetRelativeLocation(BaseLocation);

    // Scale the tip mesh for appropriate size
    ArrowTipMesh->SetRelativeScale3D(FVector(1.4f, 1.0f, 1.0f));
    // Rotate the tip mesh with the calculated look rotation plus offset
    ArrowTipMesh->SetRelativeRotation(LookAtRotation + RotationOffset);
    // Set the tip mesh to its calculated position
    ArrowTipMesh->SetRelativeLocation(TipLocation);
}

// Set the attack range of the indicator arrow
void ARocketGrabIndicator::SetAttackRange(float Range) {
    AttackRange = Range;
}
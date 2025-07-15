#include "ShurikenFlipIndicator.h"
#include "Components/DecalComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "../../../Character/BaseCharacter.h"

// Constructor: Initialize components and setup their attachments and rotations
AShurikenFlipIndicator::AShurikenFlipIndicator() {
    PrimaryActorTick.bCanEverTick = true;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    // Decal representing the attack range on the ground, rotated to face downward
    RangeDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("RangeDecal"));
    RangeDecal->SetupAttachment(RootComponent);
    RangeDecal->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));

    // Meshes for visualizing the directional arrows of the ability
    BackArrowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BackArrowMesh"));
    BackArrowMesh->SetupAttachment(RootComponent);
    BackArrowMesh->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));

    FrontArrowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FrontArrowMesh"));
    FrontArrowMesh->SetupAttachment(RootComponent);
    FrontArrowMesh->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));

    FrontArrowTipMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FrontArrowTipMesh"));
    FrontArrowTipMesh->SetupAttachment(RootComponent);
    FrontArrowTipMesh->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
}

// Updates the indicator’s orientation, position, and scale based on the given direction vector
void AShurikenFlipIndicator::UpdateIndicatorDirection(const FVector& Direction) {
    SetAttackRange(825.0f); // Fixed attack range for this ability

    FVector Forward = Direction.GetSafeNormal();

    // Calculate rotation so the indicator points along the forward direction
    FRotator LookAtRotation = UKismetMathLibrary::MakeRotFromX(Forward);
    FRotator RotationOffset(0.0f, 90.0f, 0.0f);

    // Offsets to recenter meshes relative to the forward vector for correct visual alignment
    float RecenterBaseOffset = 41.0f;
    FVector BaseOffset = Forward * RecenterBaseOffset;
    float RecenterTipOffset = 75.0f;
    FVector TipOffset = Forward * RecenterTipOffset;

    // Positions for front arrow base and tip along the forward vector
    FVector BaseLocation = Forward * (AttackRange * 0.5f) - BaseOffset;
    FVector TipLocation = Forward * AttackRange - TipOffset;

    float Distance = FVector::Dist(BaseLocation, TipLocation);

    // Set scale, rotation and position for the front arrow mesh
    FrontArrowMesh->SetRelativeScale3D(FVector(1.2f, 6.57f, 1.0f));
    FrontArrowMesh->SetRelativeRotation(LookAtRotation + RotationOffset);
    FrontArrowMesh->SetRelativeLocation(BaseLocation);
    
    // Set scale, rotation and position for the front arrow tip mesh
    FrontArrowTipMesh->SetRelativeScale3D(FVector(1.2f, 1.0f, 1.0f));
    FrontArrowTipMesh->SetRelativeRotation(LookAtRotation + RotationOffset);
    FrontArrowTipMesh->SetRelativeLocation(TipLocation);

    // Set scale, rotation and position for the back arrow mesh
    BackArrowMesh->SetRelativeScale3D(FVector(1.5f, 1.5f, 1.0f));
    BackArrowMesh->SetRelativeRotation(LookAtRotation - RotationOffset);
    BackArrowMesh->SetRelativeLocation(-Forward * 150.0f);
}

// Sets the attack range and updates the decal scale accordingly
void AShurikenFlipIndicator::SetAttackRange(float Range) {
    AttackRange = Range;
    // Scale decal size to visually represent the attack range area
    RangeDecal->SetRelativeScale3D(FVector(10.0f, AttackRange, AttackRange));
}
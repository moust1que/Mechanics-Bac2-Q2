#include "PerfectExecutionRecastIndicator.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"

APerfectExecutionRecastIndicator::APerfectExecutionRecastIndicator() {
    PrimaryActorTick.bCanEverTick = true;
    
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    
    ArrowBaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArrowBaseMesh"));
    ArrowBaseMesh->SetupAttachment(RootComponent);
    ArrowBaseMesh->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
    
    ArrowTipMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArrowTipMesh"));
    ArrowTipMesh->SetupAttachment(RootComponent);
    ArrowTipMesh->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
}

void APerfectExecutionRecastIndicator::UpdateIndicatorDirection(const FVector& Direction) {
    SetAttackRange(800.0f);

    FVector Forward = Direction.GetSafeNormal();

    FRotator LookAtRotation = UKismetMathLibrary::MakeRotFromX(Forward);

    FRotator RotationOffset(0.0f, 90.0f, 0.0f);

    float RecenterBaseOffset = 41.0f;
    FVector BaseOffset = Forward * RecenterBaseOffset;
    float RecenterTipOffset = 75.0f;
    FVector TipOffset = Forward * RecenterTipOffset;

    FVector BaseLocation = Forward * (AttackRange * 0.5f) - BaseOffset;
    FVector TipLocation = Forward * AttackRange - TipOffset;

    float Distance = FVector::Dist(BaseLocation, TipLocation);

    ArrowBaseMesh->SetRelativeScale3D(FVector(1.1f, 6.35f, 1.0f));
    ArrowBaseMesh->SetRelativeRotation(LookAtRotation + RotationOffset);
    ArrowBaseMesh->SetRelativeLocation(BaseLocation);

    ArrowTipMesh->SetRelativeScale3D(FVector(1.1f, 1.0f, 1.0f));
    ArrowTipMesh->SetRelativeRotation(LookAtRotation + RotationOffset);
    ArrowTipMesh->SetRelativeLocation(TipLocation);
}

void APerfectExecutionRecastIndicator::SetAttackRange(float Range) {
    AttackRange = Range;
}
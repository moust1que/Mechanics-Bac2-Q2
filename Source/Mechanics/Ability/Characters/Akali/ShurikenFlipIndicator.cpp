#include "ShurikenFlipIndicator.h"
#include "Components/DecalComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "../../../Character/BaseCharacter.h"

AShurikenFlipIndicator::AShurikenFlipIndicator() {
    PrimaryActorTick.bCanEverTick = true;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    RangeDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("RangeDecal"));
    RangeDecal->SetupAttachment(RootComponent);
    RangeDecal->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));

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

void AShurikenFlipIndicator::UpdateIndicatorDirection(const FVector& Direction) {
    SetAttackRange(825.0f);

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

    FrontArrowMesh->SetRelativeScale3D(FVector(1.2f, 6.57f, 1.0f));
    FrontArrowMesh->SetRelativeRotation(LookAtRotation + RotationOffset);
    FrontArrowMesh->SetRelativeLocation(BaseLocation);

    FrontArrowTipMesh->SetRelativeScale3D(FVector(1.2f, 1.0f, 1.0f));
    FrontArrowTipMesh->SetRelativeRotation(LookAtRotation + RotationOffset);
    FrontArrowTipMesh->SetRelativeLocation(TipLocation);

    BackArrowMesh->SetRelativeScale3D(FVector(1.5f, 1.5f, 1.0f));
    BackArrowMesh->SetRelativeRotation(LookAtRotation - RotationOffset);
    BackArrowMesh->SetRelativeLocation(-Forward * 150.0f);
}

void AShurikenFlipIndicator::SetAttackRange(float Range) {
    AttackRange = Range;
    RangeDecal->SetRelativeScale3D(FVector(10.0f, AttackRange, AttackRange));
}
#include "ShurikenFlipTargetingIndicator.h"
#include "Components/DecalComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"

AShurikenFlipTargetingIndicator::AShurikenFlipTargetingIndicator() {
	PrimaryActorTick.bCanEverTick = false;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    RangeDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("RangeDecal"));
    RangeDecal->SetupAttachment(RootComponent);
    RangeDecal->DecalSize = FVector(AttackRange, AttackRange, 1.0f);
    RangeDecal->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));

    BackArrowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BackArrowMesh"));
    BackArrowMesh->SetupAttachment(RootComponent);

    FrontArrowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FrontArrowMesh"));
    FrontArrowMesh->SetupAttachment(RootComponent);

    FrontArrowTipMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FrontArrowTipMesh"));
    FrontArrowTipMesh->SetupAttachment(RootComponent);
}

// void AShurikenFlipTargetingIndicator::BeginPlay() {
// 	Super::BeginPlay();
// }

// void AShurikenFlipTargetingIndicator::Tick(float DeltaTime) {
// 	Super::Tick(DeltaTime);
// }

void AShurikenFlipTargetingIndicator::UpdateIndicatorDirection(const FVector& Direction) {
    FVector Forward = Direction.GetSafeNormal();

    FRotator LookAtRotation = UKismetMathLibrary::MakeRotFromX(Forward);

    FrontArrowMesh->SetWorldRotation(LookAtRotation);
    FrontArrowMesh->SetRelativeLocation(Forward * (AttackRange * 0.5f));

    FrontArrowTipMesh->SetWorldRotation(LookAtRotation);
    FrontArrowTipMesh->SetRelativeLocation(Forward * AttackRange);

    BackArrowMesh->SetWorldRotation(LookAtRotation + FRotator(0.0f, 180.0f, 0.0f));
    BackArrowMesh->SetRelativeLocation(-Forward * 150.0f);
}

void AShurikenFlipTargetingIndicator::SetAttackRange(float Range) {
    AttackRange = Range;
    RangeDecal->DecalSize = FVector(Range, Range, 1.0f);
}

void AShurikenFlipTargetingIndicator::ShowIndicator(bool Show) {
    SetActorHiddenInGame(!Show);
    SetActorEnableCollision(false);
    SetActorTickEnabled(Show);
}
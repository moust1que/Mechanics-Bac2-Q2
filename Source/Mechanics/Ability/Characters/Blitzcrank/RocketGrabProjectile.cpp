#include "RocketGrabProjectile.h"
// #include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "../../../Character/BaseCharacter.h"

ARocketGrabProjectile::ARocketGrabProjectile() {
	PrimaryActorTick.bCanEverTick = true;

    CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
    CollisionComponent->SetBoxExtent(FVector(1.0f, 60.0f, 1.0f));
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CollisionComponent->SetCollisionObjectType(ECC_WorldDynamic);
    CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
    CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
    CollisionComponent->OnComponentHit.AddDynamic(this, &ARocketGrabProjectile::OnHit);
    RootComponent = CollisionComponent;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere"));
    MeshComponent->SetStaticMesh(SphereMesh.Object);
    MeshComponent->SetRelativeScale3D(FVector(0.5f, 0.4f, 0.5f));
    MeshComponent->SetupAttachment(RootComponent);

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->InitialSpeed = 1800.0f;
    ProjectileMovement->MaxSpeed = 1800.0f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;
}

void ARocketGrabProjectile::BeginPlay() {
	Super::BeginPlay();

    SpawnLocation = GetActorLocation();
}

void ARocketGrabProjectile::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

    float Distance = FVector::Dist(SpawnLocation, GetActorLocation());
    if(!HasHit && Distance >= MaxRange && !ProjectileDisabled)  {
        ProjectileDisabled = true;

        ProjectileMovement->StopMovementImmediately();
        ProjectileMovement->Deactivate();

        ABaseCharacter* OwnerCharacter = Cast<ABaseCharacter>(GetOwner());
        if(OwnerCharacter) OwnerCharacter->CanMove = true;

        DestroyProjectile();
    }

    if(IsPulling && TargetCharacter) {
        PullTimer += DeltaTime;
        float Alpha = FMath::Clamp(PullTimer / PullTime, 0.0f, 1.0f);
        FVector NewLocation = FMath::Lerp(PullStartLocation, PullEndLocation, Alpha);

        SetActorLocation(NewLocation);
        TargetCharacter->SetActorLocation(NewLocation, true);

        if(Alpha >= 1.0f) {
            IsPulling = false;
            TargetCharacter->CanMove = true;
            TargetCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

            ABaseCharacter* OwnerCharacter = Cast<ABaseCharacter>(GetOwner());
            if(OwnerCharacter) OwnerCharacter->CanMove = true;

            DestroyProjectile();
        }
    }
}

void ARocketGrabProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
    if(OtherActor && OtherActor != GetOwner()) {
        ABaseCharacter* Target = Cast<ABaseCharacter>(OtherActor);
        if(!HasHit && Target) {
            HasHit = true;
            ProjectileMovement->StopMovementImmediately();
            ProjectileMovement->Deactivate();

            StartPull(Target, GetOwner()->GetActorLocation());

            if(OnGrappleHit.IsBound()) {
                OnGrappleHit.Execute(OtherActor, Hit.ImpactPoint);
            }
        }
    }
}

void ARocketGrabProjectile::DestroyProjectile() {
    Destroy();
}

void ARocketGrabProjectile::StartPull(ABaseCharacter* Target, const FVector& Destination) {
    TargetCharacter = Target;
    TargetCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    PullStartLocation = TargetCharacter->GetActorLocation();

    FVector OwnerForward = GetOwner()->GetActorForwardVector();

    PullEndLocation = Destination + OwnerForward * 8.0f;

    PullTimer = 0.0f;
    IsPulling = true;

    TargetCharacter->CanMove = false;
}
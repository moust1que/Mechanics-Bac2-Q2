#include "RocketGrabProjectile.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "../../../Character/BaseCharacter.h"

ARocketGrabProjectile::ARocketGrabProjectile() {
	PrimaryActorTick.bCanEverTick = true;

    // Create collision box for hit detection
    CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
    CollisionComponent->SetBoxExtent(FVector(1.0f, 60.0f, 1.0f));
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CollisionComponent->SetCollisionObjectType(ECC_WorldDynamic);
    CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
    CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
    CollisionComponent->OnComponentHit.AddDynamic(this, &ARocketGrabProjectile::OnHit);
    RootComponent = CollisionComponent;

    // Mesh for visual representation
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere"));
    MeshComponent->SetStaticMesh(SphereMesh.Object);
    MeshComponent->SetRelativeScale3D(FVector(0.5f, 0.4f, 0.5f));
    MeshComponent->SetupAttachment(RootComponent);

    // Handle projectile movement
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->InitialSpeed = 1800.0f;
    ProjectileMovement->MaxSpeed = 1800.0f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;
}

// Called when the projectile is spawned
void ARocketGrabProjectile::BeginPlay() {
	Super::BeginPlay();

    SpawnLocation = GetActorLocation(); // Save initial location for range tracking
}

void ARocketGrabProjectile::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

    // Check if the projectile has exceeded its max range
    float Distance = FVector::Dist(SpawnLocation, GetActorLocation());
    if(!HasHit && Distance >= MaxRange && !ProjectileDisabled)  {
        ProjectileDisabled = true;

        ProjectileMovement->StopMovementImmediately();
        ProjectileMovement->Deactivate();

        // Re-enable movement for the owner if nothing was hit
        ABaseCharacter* OwnerCharacter = Cast<ABaseCharacter>(GetOwner());
        if(OwnerCharacter) OwnerCharacter->CanMove = true;

        DestroyProjectile(); // Clean up
    }

    // Handle pulling the target back to the caster
    if(IsPulling && TargetCharacter) {
        PullTimer += DeltaTime;
        float Alpha = FMath::Clamp(PullTimer / PullTime, 0.0f, 1.0f);
        // Interpolate between pull start and end location
        FVector NewLocation = FMath::Lerp(PullStartLocation, PullEndLocation, Alpha);

        SetActorLocation(NewLocation); // Move projectile
        TargetCharacter->SetActorLocation(NewLocation, true); // Move target with projectile

        // If pull is complete
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

// Called when the projectile hits something
void ARocketGrabProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
    // Ignore self and owner
    if(OtherActor && OtherActor != GetOwner()) {
        ABaseCharacter* Target = Cast<ABaseCharacter>(OtherActor);
        if(!HasHit && Target) {
            HasHit = true;
            ProjectileMovement->StopMovementImmediately();
            ProjectileMovement->Deactivate();

            // Begin pull toward caster
            StartPull(Target, GetOwner()->GetActorLocation());

            if(OnGrappleHit.IsBound()) {
                OnGrappleHit.Execute(OtherActor, Hit.ImpactPoint); // Notify ability logic
            }
        }
    }
}

void ARocketGrabProjectile::DestroyProjectile() {
    Destroy();
}

// Start pulling a character back to the owner
void ARocketGrabProjectile::StartPull(ABaseCharacter* Target, const FVector& Destination) {
    TargetCharacter = Target;
    // Disable collision for the pulled character to avoid getting stuck
    TargetCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    PullStartLocation = TargetCharacter->GetActorLocation();

    FVector OwnerForward = GetOwner()->GetActorForwardVector();

    PullEndLocation = Destination + OwnerForward * 8.0f; // Pull slightly in front of the owner

    PullTimer = 0.0f;
    IsPulling = true;

    TargetCharacter->CanMove = false; // Freeze movement while being pulled
}
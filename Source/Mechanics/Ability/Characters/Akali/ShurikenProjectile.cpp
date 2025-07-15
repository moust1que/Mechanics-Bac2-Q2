#include "ShurikenProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "../../../Character/BaseCharacter.h"

// Constructor: Setup components and initial values for the shuriken projectile
AShurikenProjectile::AShurikenProjectile() {
	PrimaryActorTick.bCanEverTick = true;

    // Collision component setup: sphere shape, collision responses
    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    CollisionComponent->InitSphereRadius(16.0f);
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CollisionComponent->SetCollisionObjectType(ECC_WorldDynamic);
    CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
    CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
    CollisionComponent->OnComponentHit.AddDynamic(this, &AShurikenProjectile::OnHit);
    RootComponent = CollisionComponent;

    // Visual mesh component setup with basic sphere mesh, scaled down
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere"));
    MeshComponent->SetStaticMesh(SphereMesh.Object);
    MeshComponent->SetRelativeScale3D(FVector(0.2f, 0.2f, 0.2f));
    MeshComponent->SetupAttachment(RootComponent);

    // Projectile movement component setup: speed, behavior
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->InitialSpeed = 1800.0f;
    ProjectileMovement->MaxSpeed = 1800.0f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;

    InitialLifeSpan = 5.0f;
}

// Called when the projectile is spawned in the world
void AShurikenProjectile::BeginPlay() {
    Super::BeginPlay();

    SpawnLocation = GetActorLocation();
}

// Tick function: handles max range checking and disables projectile when exceeded
void AShurikenProjectile::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    float Distance = FVector::Dist(SpawnLocation, GetActorLocation());
    if(!HasHit && Distance >= MaxRange && !ProjectileDisabled) {
        ProjectileDisabled = true;

        // Stop movement and deactivate projectile movement component
        ProjectileMovement->StopMovementImmediately();
        ProjectileMovement->Deactivate();

        // Adjust position and disable physics/gravity to freeze projectile in place
        FVector CurrentLocation = GetActorLocation();
        SetActorLocation(FVector(CurrentLocation.X, CurrentLocation.Y, 8.0f));

        CollisionComponent->SetSimulatePhysics(false);
        CollisionComponent->SetEnableGravity(false);
        SetActorEnableCollision(true);

        // Schedule destruction after a delay
        FTimerHandle DestroyTimerHandle;
        GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, this, &AShurikenProjectile::DestroyProjectile, 2.5f, false);

        // Trigger OnShurikenNoHit delegate if bound
        if(OnShurikenNoHit.IsBound()) {
            OnShurikenNoHit.Execute();
        }
    }
}

// Called when the projectile hits an actor. Triggers damage and destruction
void AShurikenProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
    if(OtherActor && OtherActor != GetOwner() && Cast<ABaseCharacter>(OtherActor)) {
        HasHit = true;
        if(OnShurikenHit.IsBound()) {
            OnShurikenHit.Execute(OtherActor, Hit.ImpactPoint);
        }
        DestroyProjectile();
    }
}

// Destroy the projectile actor
void AShurikenProjectile::DestroyProjectile() {
    Destroy();
}
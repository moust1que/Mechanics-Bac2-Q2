#include "ShurikenProjectile.h"
#include "Components/SphereComponent.h"
// #include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "../../../Character/BaseCharacter.h"

AShurikenProjectile::AShurikenProjectile() {
	PrimaryActorTick.bCanEverTick = true;

    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    CollisionComponent->InitSphereRadius(16.0f);
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CollisionComponent->SetCollisionObjectType(ECC_WorldDynamic);
    CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
    CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
    CollisionComponent->OnComponentHit.AddDynamic(this, &AShurikenProjectile::OnHit);
    RootComponent = CollisionComponent;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere"));
    MeshComponent->SetStaticMesh(SphereMesh.Object);
    MeshComponent->SetRelativeScale3D(FVector(0.2f, 0.2f, 0.2f));
    MeshComponent->SetupAttachment(RootComponent);

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->InitialSpeed = 1800.0f;
    ProjectileMovement->MaxSpeed = 1800.0f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;

    InitialLifeSpan = 5.0f;
}

void AShurikenProjectile::BeginPlay() {
    Super::BeginPlay();

    SpawnLocation = GetActorLocation();
}

void AShurikenProjectile::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    float Distance = FVector::Dist(SpawnLocation, GetActorLocation());
    if(!HasHit && Distance >= MaxRange && !ProjectileDisabled) {
        ProjectileDisabled = true;

        ProjectileMovement->StopMovementImmediately();
        ProjectileMovement->Deactivate();

        FVector CurrentLocation = GetActorLocation();
        SetActorLocation(FVector(CurrentLocation.X, CurrentLocation.Y, 8.0f));

        CollisionComponent->SetSimulatePhysics(false);
        CollisionComponent->SetEnableGravity(false);
        SetActorEnableCollision(true);

        FTimerHandle DestroyTimerHandle;
        GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, this, &AShurikenProjectile::DestroyProjectile, 2.5f, false);

        if(OnShurikenNoHit.IsBound()) {
            OnShurikenNoHit.Execute();
        }
    }
}

void AShurikenProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
    if(OtherActor && OtherActor != GetOwner() && Cast<ABaseCharacter>(OtherActor)) {
        HasHit = true;
        if(OnShurikenHit.IsBound()) {
            OnShurikenHit.Execute(OtherActor, Hit.ImpactPoint);
        }
        DestroyProjectile();
    }
}

void AShurikenProjectile::DestroyProjectile() {
    Destroy();
}
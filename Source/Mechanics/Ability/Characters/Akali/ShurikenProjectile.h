#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShurikenProjectile.generated.h"

// Delegate for when shuriken hits a target, passing the hit actor and hit location
DECLARE_DELEGATE_TwoParams(FShurikenHitSignature, AActor*, FVector);
// Delegate for when shuriken misses all targets
DECLARE_DELEGATE(FShurikenNoHit);

// Forward declaration
class UProjectileMovementComponent;
class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class MECHANICS_API AShurikenProjectile : public AActor {
	GENERATED_BODY()
	
    public:	
        AShurikenProjectile();

        // Movement component handling projectile velocity and behavior
        UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UProjectileMovementComponent* ProjectileMovement;

        // Collision component used for detecting hits
        UPROPERTY(VisibleAnywhere, BlueprintReadOnly) USphereComponent* CollisionComponent;

        // Visual mesh component representing the shuriken
        UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UStaticMeshComponent* MeshComponent;

        // Delegate called on hit event
        FShurikenHitSignature OnShurikenHit;
        // Delegate called when no hit is registered
        FShurikenNoHit OnShurikenNoHit;

        virtual void Tick(float DeltaTime) override;

    protected:
        UFUNCTION() void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
        virtual void BeginPlay() override;

    private:
        // Maximum travel range before projectile is disabled
        float MaxRange = 825.0f / 1.25f;

        // Location where the projectile was spawned
        FVector SpawnLocation;
        // Has the projectile already hit a target?
        bool HasHit = false;
        // Has the projectile been disabled
        bool ProjectileDisabled = false;

        UFUNCTION() void DestroyProjectile();
};
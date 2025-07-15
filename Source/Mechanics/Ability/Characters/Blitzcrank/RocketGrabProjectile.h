#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RocketGrabProjectile.generated.h"

// Delegate called when the grapple hits an actor
DECLARE_DELEGATE_TwoParams(FGrappleHitSignature, AActor*, FVector);

// Forward declaration
class UProjectileMovementComponent;
class UBoxComponent;
class UStaticMeshComponent;
class ABaseCharacter;

UCLASS()
class MECHANICS_API ARocketGrabProjectile : public AActor {
	GENERATED_BODY()
	
    public:
        ARocketGrabProjectile();

        // Handles projectile movement
        UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UProjectileMovementComponent* ProjectileMovement;

        // Box used for hit collision detection
        UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UBoxComponent* CollisionComponent;

        // Visual representation of the projectile
        UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UStaticMeshComponent* MeshComponent;

        // Delegate called when the projectile successfully hits a target
        FGrappleHitSignature OnGrappleHit;

        // Character that is currently being pulled
        UPROPERTY() ABaseCharacter* TargetCharacter = nullptr;

        // Pulling system variables
        FVector PullStartLocation;
        FVector PullEndLocation;
        float PullTime = 0.5f;
        float PullTimer = 0.0f;
        bool IsPulling = false;

        void StartPull(ABaseCharacter* Target, const FVector& Destination);

        virtual void Tick(float DeltaTime) override;

    protected:
        virtual void BeginPlay() override;

        UFUNCTION() void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    private:
        // Maximum distance the projectile can travel before deactivating
        float MaxRange = 1020.0f;

        // Starting location used to compute travel distance
        FVector SpawnLocation;
        // Flags for managing projectile state
        bool HasHit = false;
        bool ProjectileDisabled = false;

        void DestroyProjectile();
};
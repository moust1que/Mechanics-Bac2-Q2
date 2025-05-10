#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RocketGrabProjectile.generated.h"

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

        UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UProjectileMovementComponent* ProjectileMovement;

        UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UBoxComponent* CollisionComponent;

        UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UStaticMeshComponent* MeshComponent;

        FGrappleHitSignature OnGrappleHit;

        UPROPERTY() ABaseCharacter* TargetCharacter = nullptr;

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
        float MaxRange = 1020.0f;

        FVector SpawnLocation;
        bool HasHit = false;
        bool ProjectileDisabled = false;

        void DestroyProjectile();
};
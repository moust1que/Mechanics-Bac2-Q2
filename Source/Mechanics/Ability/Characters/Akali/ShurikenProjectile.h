#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShurikenProjectile.generated.h"

DECLARE_DELEGATE_TwoParams(FShurikenHitSignature, AActor*, FVector);
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

        UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UProjectileMovementComponent* ProjectileMovement;

        UPROPERTY(VisibleAnywhere, BlueprintReadOnly) USphereComponent* CollisionComponent;

        UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UStaticMeshComponent* MeshComponent;

        FShurikenHitSignature OnShurikenHit;
        FShurikenNoHit OnShurikenNoHit;

        virtual void Tick(float DeltaTime) override;

    protected:
        UFUNCTION() void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
        virtual void BeginPlay() override;

    private:
        float MaxRange = 825.0f / 1.2f;

        FVector SpawnLocation;
        bool HasHit = false;
        bool ProjectileDisabled = false;

        UFUNCTION() void DestroyProjectile();
};
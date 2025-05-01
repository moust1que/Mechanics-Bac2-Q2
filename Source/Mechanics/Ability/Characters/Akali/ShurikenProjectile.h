#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShurikenProjectile.generated.h"

DECLARE_DELEGATE_TwoParams(FShurikenHitSignature, AActor*, FVector);

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

    protected:
        UFUNCTION() void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
#pragma once

#include "CoreMinimal.h"
#include "../../AbilityBase.h"
#include "../../../Character/Characters/Blitzcrank.h"
#include "RocketGrab.generated.h"

UCLASS()
class MECHANICS_API URocketGrab : public UAbilityBase {
	GENERATED_BODY()

    public:
        URocketGrab();

        UFUNCTION() void ActivateAbility() override;
        void UpdateStats() override;
        TArray<float> GetArguments() override;
        UFUNCTION() void LaunchAttack() override;

        UFUNCTION() void OnGrappleHit(AActor* HitActor, FVector HitLocation);

        // Reference to the Blitzcrank character casting this ability, for access to stats like AbilityPower
        UPROPERTY() ABlitzcrank* BlitzcrankCharacter = nullptr;
};
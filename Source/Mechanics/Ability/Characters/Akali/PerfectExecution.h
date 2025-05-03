#pragma once

#include "CoreMinimal.h"
#include "../../AbilityBase.h"
#include "../../../Character/Characters/Akali.h"
#include "PerfectExecution.generated.h"

UCLASS()
class MECHANICS_API UPerfectExecution : public UAbilityBase {
	GENERATED_BODY()

    public:
        UPerfectExecution();
        UFUNCTION() void ActivateAbility() override;
        void UpdateStats() override;
        UFUNCTION() void StartCooldown() override;
        TArray<float> GetArguments() override;

        UFUNCTION() void PerformRecast();
        UFUNCTION() void CancelRecast();

        UFUNCTION() void ResetCooldown();

        UPROPERTY(EditAnywhere, Category = Ability) float MinBaseDamage;
        UPROPERTY(EditAnywhere, Category = Ability) float MaxBaseDamage;

        UPROPERTY() AAkali* AkaliCharacter = nullptr;
};
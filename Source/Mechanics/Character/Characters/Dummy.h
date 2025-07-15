#pragma once

#include "CoreMinimal.h"
#include "../BaseCharacter.h"
#include "Dummy.generated.h"

UCLASS()
class MECHANICS_API ADummy : public ABaseCharacter {
	GENERATED_BODY()

    public:
        ADummy();

        void ReceiveDamage(float Damage) override;

        // Timer handle for scheduling health regeneration
        FTimerHandle RegenTimerHandle;
        // Delay before health regeneration begins after damage
        float RegenDelay = 3.0f;

    protected:
        virtual void BeginPlay() override;

    private:
        void RegenHealth();
};
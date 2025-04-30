#pragma once

#include "CoreMinimal.h"
#include "../BaseCharacter.h"
#include "Akali.generated.h"

UCLASS()
class MECHANICS_API AAkali : public ABaseCharacter {
	GENERATED_BODY()

    public:
        AAkali();

        UFUNCTION(BlueprintCallable, Category = Character) void LevelUP() override;

    private:
        UFUNCTION() void UpdateStats() override;
};
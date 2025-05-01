#pragma once

#include "CoreMinimal.h"
#include "../BaseCharacter.h"
#include "Blitzcrank.generated.h"

UCLASS()
class MECHANICS_API ABlitzcrank : public ABaseCharacter {
	GENERATED_BODY()

    public:
        UFUNCTION(BlueprintCallable, Category = Character) void LevelUP() override;

    protected:
        virtual void BeginPlay() override;

    private:
        UFUNCTION() void UpdateStats() override;
};
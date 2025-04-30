#include "ShurikenFlip.h"
#include "../../../Character/BaseCharacter.h"

UShurikenFlip::UShurikenFlip() {
    Level = 0;
    MaxLevel = 5;
    BaseCooldown = 16.0f;
    BaseRessourceCost = 30.0f;
}

void UShurikenFlip::UpdateStats() {
    TotBaseDamage = 70.0f * Level;
    Cooldown = BaseCooldown + 1.5f * (Level - 1);
    RessourceCost = BaseRessourceCost;
    BaseDamage = TotBaseDamage * 0.3f;
    AbilityDamage = CurCharacter->AbilityDamage * 0.3f;
    AbilityPower = CurCharacter->AbilityPower * 0.33f;
    TotalDamage = BaseDamage + AbilityDamage + AbilityPower;
    
    RecastBaseDamage = TotBaseDamage * 0.7f;
    RecastAbilityDamage = CurCharacter->AbilityDamage * 0.7f;
    RecastAbilityPower = CurCharacter->AbilityPower * 0.77f;
    RecastTotalDamage = RecastBaseDamage + RecastAbilityDamage + RecastAbilityPower;
}

void UShurikenFlip::ActivateAbility() {
    UE_LOG(LogTemp, Warning, TEXT("Shuriken Flip Activated"));
}
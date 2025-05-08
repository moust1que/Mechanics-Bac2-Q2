#include "Blitzcrank.h"

void ABlitzcrank::BeginPlay() {
    Super::BeginPlay();

    Level = 1;
    BaseHealth = 600.0f;
    Health = BaseHealth;
    HealthPerLevel = 109.0f;
    BaseRessource = 267.0f;
    Ressource = BaseRessource;
    RessourcePerLevel = 40.0f;
    BaseAbilityDamage = 62.0f;
    AbilityDamagePerLevel = 3.5f;
    BaseAbilityPower = 0.0f;
    AbilityPowerPerLevel = 0.0f;
    BaseAttackSpeed = 0.625f;
    AttackSpeedRatio = 1.13f;
    BasePhysicalResist = 37.0f;
    PhysicalResistPerLevel = 4.7f;
    BaseMagicResist = 32.0f;
    MagicResistPerLevel = 2.05f;
    MoveSpeed = 325.0f;
    AttackRange = 125.0f;
    BaseHealthRegen = 7.5f;
    HealthRegenPerLevel = 0.75f;
    BaseRessourceRegen = 8.5f;
    RessourceRegenPerLevel = 0.8f;
    UpdateStats();

    RessourceType = ERessourceType::Mana;
    if(HealthBarWidget) {
        HealthBarWidget->DisplayResourceBar(RessourceType);
    }

    Cast<ABaseCharacter>(this)->LaunchRegen();
}

void ABlitzcrank::UpdateStats() {
    MaxHealth = BaseHealth + HealthPerLevel * (Level - 1);
    MaxRessource = BaseRessource + RessourcePerLevel * (Level - 1);
    AbilityDamage = BaseAbilityDamage + AbilityDamagePerLevel * (Level - 1);
    AbilityPower = BaseAbilityPower + AbilityPowerPerLevel * (Level - 1);
    AttackSpeed = BaseAttackSpeed * (1 + (Level - 1) * (AttackSpeedRatio / 100.0f));
    PhysicalResist = BasePhysicalResist + PhysicalResistPerLevel * (Level - 1);
    MagicResist = BaseMagicResist + MagicResistPerLevel * (Level - 1);
    HealthRegen = BaseHealthRegen + HealthRegenPerLevel * (Level - 1);
    RessourceRegen = BaseRessourceRegen + RessourceRegenPerLevel * (Level - 1);
}

// void ABlitzcrank::LevelUP() {
//     if(Level < MaxLevel) {
//         Level++;
//         Health += HealthPerLevel;
//         Ressource += RessourcePerLevel;
//         UpdateStats();
//         HealthBarWidget->UpdateHealthOnChange(Health, MaxHealth);
//         HealthBarWidget->UpdateResourceOnChange(Ressource, MaxRessource);
//     }
// }
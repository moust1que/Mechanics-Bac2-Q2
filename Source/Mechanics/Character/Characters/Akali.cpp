#include "Akali.h"

AAkali::AAkali() {
    Level = 1;
    BaseHealth = 600.0f;
    Health = BaseHealth;
    HealthPerLevel = 119.0f;
    MaxRessource = 200.0f;
    Ressource = BaseRessource;
    RessourcePerLevel = 0.0f;
    BaseAbilityDamage = 62.0f;
    AbilityDamagePerLevel = 3.3f;
    BaseAbilityPower = 0.0f;
    AbilityPowerPerLevel = 0.0f;
    BaseAttackSpeed = 0.625f;
    AttackSpeedRatio = 3.2f;
    BasePhysicalResist = 23.0f;
    PhysicalResistPerLevel = 4.7f;
    BaseMagicResist = 37.0f;
    MagicResistPerLevel = 2.05f;
    MoveSpeed = 345.0f;
    AttackRange = 125.0f;
    BaseHealthRegen = 9.0f;
    HealthRegenPerLevel = 0.9f;
    BaseRessourceRegen = 50.0f;
    RessourceRegenPerLevel = 0.0f;
    UpdateStats();
}

void AAkali::UpdateStats() {
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

void AAkali::LevelUP() {
    if(Level < MaxLevel) {
        Level++;
        Health += HealthPerLevel;
        Ressource += RessourcePerLevel;
        UpdateStats();
    }
}
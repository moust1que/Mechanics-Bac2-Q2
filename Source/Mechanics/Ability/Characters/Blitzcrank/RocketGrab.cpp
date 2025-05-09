#include "RocketGrab.h"

URocketGrab::URocketGrab() {
    Level = 0;
    AbilityID = 0;
    MaxLevel = 5;
    BaseCooldown = 20.0f;
    BaseRessourceCost = 100.0f;
}

void URocketGrab::UpdateStats() {
    BlitzcrankCharacter = Cast<ABlitzcrank>(CurCharacter);

    Cooldown = BaseCooldown - 1.0f * (Level - 1);
    RessourceCost = BaseRessourceCost;
    BaseDamage = 105.0f + 45.0f * Level;
    AbilityDamage = 0.0f;
    AbilityPower = BlitzcrankCharacter->AbilityPower * 1.2f;
    TotalDamage = BaseDamage + AbilityDamage + AbilityPower;

    Arg1 = TotalDamage;
}

void URocketGrab::ActivateAbility() {
    if(Level == 0) return;

    if(IsOnCooldown) return;

    StartCooldown(Cooldown);
}

TArray<float> URocketGrab::GetArguments() {
    UpdateStats();
    return { Arg1, Arg2, Arg3 };
}
#include "PerfectExecution.h"

UPerfectExecution::UPerfectExecution() {
    Level = 0;
    AbilityID = 3;
    MaxLevel = 3;
    BaseCooldown = 120.0f;
    BaseRessourceCost = 0.0f;
}

void UPerfectExecution::UpdateStats() {
    AkaliCharacter = Cast<AAkali>(CurCharacter);
    
    MinBaseDamage = Level == 0 ? 70.0f : 70.0f * Level;
    MaxBaseDamage = Level == 0 ? 210.0f : 210.0f * Level;
    Cooldown = BaseCooldown - 30.0f * (Level - 1);
    RessourceCost = BaseRessourceCost;
    BaseDamage = Level == 0 ? 110.0f : 110.0f * Level;
    AbilityDamage = 0.0f;
    AbilityPower = AkaliCharacter->AbilityPower * 0.3f;
    TotalDamage = BaseDamage + AbilityDamage + AbilityPower;
    
    RecastBaseDamage = MinBaseDamage + ((MinBaseDamage - MaxBaseDamage) / 0.7f) * (100 / 100 - 1);
    RecastAbilityDamage = 0.0f;
    RecastAbilityPower = AkaliCharacter->AbilityPower * 0.9f;
    RecastTotalDamage = RecastBaseDamage + RecastAbilityDamage + RecastAbilityPower;

    Arg1 = TotalDamage;
    Arg2 = MinBaseDamage;
    Arg3 = MaxBaseDamage;
}

void UPerfectExecution::ActivateAbility() {
    if(Level == 0) return;

    if(IsOnCooldown) return;

    if(CanRecast) {
        PerformRecast();
        return;
    }

    StartCooldown();
}

void UPerfectExecution::PerformRecast() {
    
}

void UPerfectExecution::CancelRecast() {
    CanRecast = false;
    // RecastTarget = nullptr;   
}

void UPerfectExecution::ResetCooldown() {
    IsOnCooldown = false;
}

void UPerfectExecution::StartCooldown() {
    IsOnCooldown = true;
    CurCharacter->GetWorld()->GetTimerManager().SetTimer(CooldownTimer, this, &UPerfectExecution::ResetCooldown, Cooldown, false);

    CurCharacter->HUDWidget->StartCooldown(this);
}

TArray<float> UPerfectExecution::GetArguments() {
    UpdateStats();
    return { Arg1, Arg2, Arg3 };
}
#include "AbilityBase.h"
#include "../Character/BaseCharacter.h"

// Starts a timer to delay the execution of an ability function
void UAbilityBase::StartCastTimer(float CastDuration, FName FunctionName) {
    FTimerDelegate TimerDel;
    TimerDel.BindUFunction(this, FunctionName);

    FTimerHandle CastTimerHandle;
    // Set a timer that will call the specified function after the given cast duration
    CurCharacter->GetWorld()->GetTimerManager().SetTimer(CastTimerHandle, TimerDel, CastDuration, false);
}

// Starts the cooldown for the ability, unless cooldowns are being auto-refreshed
void UAbilityBase::StartCooldown(float CooldownToUse, bool IsSkipable) {
    // Skip cooldown if auto-refresh is enabled and the cooldown is skippable
    if(CurCharacter->AutoRefreshCooldowns && IsSkipable) {
        IsOnCooldown = false;
        // Ensure any existing cooldown timer is cleared
        CurCharacter->GetWorld()->GetTimerManager().ClearTimer(CooldownTimer);
        // Notify the HUD to reset the visual cooldown
        CurCharacter->HUDWidget->ResetCooldown(this);
        return;
    }

    // Start the actual cooldown
    IsOnCooldown = true;
    // Schedule ResetCooldown to be called after the specified duration
    CurCharacter->GetWorld()->GetTimerManager().SetTimer(CooldownTimer, this, &UAbilityBase::ResetCooldown, CooldownToUse, false);

    // Notify HUD to visually start the cooldown
    CurCharacter->HUDWidget->StartCooldown(this, CooldownToUse);
}

// Resets the cooldown state of the ability
void UAbilityBase::ResetCooldown() {
    IsOnCooldown = false;
    CanBeUsed = true;

    // Clear the cooldown timer to avoid leftover handles
    CurCharacter->GetWorld()->GetTimerManager().ClearTimer(CooldownTimer);

    // Notify HUD to reset cooldown visuals
    CurCharacter->HUDWidget->ResetCooldown(this);
}
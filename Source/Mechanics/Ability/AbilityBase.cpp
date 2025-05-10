#include "AbilityBase.h"
#include "../Character/BaseCharacter.h"

void UAbilityBase::StartCastTimer(float CastDuration, FName FunctionName) {
    FTimerDelegate TimerDel;
    TimerDel.BindUFunction(this, FunctionName);

    FTimerHandle CastTimerHandle;
    CurCharacter->GetWorld()->GetTimerManager().SetTimer(CastTimerHandle, TimerDel, CastDuration, false);
}

void UAbilityBase::StartCooldown(float CooldownToUse, bool IsSkipable) {
    if(CurCharacter->AutoRefreshCooldowns && IsSkipable) {
        IsOnCooldown = false;
        CurCharacter->GetWorld()->GetTimerManager().ClearTimer(CooldownTimer);
        CurCharacter->HUDWidget->ResetCooldown(this);
        return;
    }

    IsOnCooldown = true;
    CurCharacter->GetWorld()->GetTimerManager().SetTimer(CooldownTimer, this, &UAbilityBase::ResetCooldown, CooldownToUse, false);

    CurCharacter->HUDWidget->StartCooldown(this, CooldownToUse);
}

void UAbilityBase::ResetCooldown() {
    IsOnCooldown = false;
    CanBeUsed = true;

    CurCharacter->GetWorld()->GetTimerManager().ClearTimer(CooldownTimer);

    CurCharacter->HUDWidget->ResetCooldown(this);
}
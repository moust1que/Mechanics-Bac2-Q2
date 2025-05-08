#include "Dummy.h"

ADummy::ADummy() {
    IsEnemy = true;
}

void ADummy::BeginPlay() {
    Super::BeginPlay();
    
    RessourceType = ERessourceType::Timer;
    if(HealthBarWidget) {
        HealthBarWidget->DisplayResourceBar(RessourceType);
    }
}

void ADummy::ReceiveDamage(float Damage) {
    Health -= Damage;
    Health = FMath::Clamp(Health, 1.0f, MaxHealth);
    HealthBarWidget->UpdateHealthOnChange(Health, MaxHealth);
    HealthBarWidget->StartHealthReload();

    GetWorld()->GetTimerManager().ClearTimer(RegenTimerHandle);
    GetWorld()->GetTimerManager().SetTimer(RegenTimerHandle, this, &ADummy::RegenHealth, RegenDelay, false);
}

void ADummy::RegenHealth() {
    Health = MaxHealth;
    HealthBarWidget->UpdateHealthOnChange(Health, MaxHealth);
}
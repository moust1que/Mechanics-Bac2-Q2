#include "Dummy.h"

ADummy::ADummy() {
    // Set this dummy as an enemy by default
    IsEnemy = true;
}

void ADummy::BeginPlay() {
    Super::BeginPlay();
    
    // Set the resource type to Timer
    RessourceType = ERessourceType::Timer;
    // Show the resource bar if the widget is available
    if(HealthBarWidget) {
        HealthBarWidget->DisplayResourceBar(RessourceType);
    }
}

void ADummy::ReceiveDamage(float Damage) {
    // Decrease health by damage amount
    Health -= Damage;
    // Clamp health so it never goes below 1 or above max health
    Health = FMath::Clamp(Health, 1.0f, MaxHealth);
    // Update the health bar UI to reflect new health
    HealthBarWidget->UpdateHealthOnChange(Health, MaxHealth);
    // Start the health bar reload animation
    HealthBarWidget->StartHealthReload();

    // Reset any previous regen timer to avoid stacking
    GetWorld()->GetTimerManager().ClearTimer(RegenTimerHandle);
    // Start a new timer to trigger health regeneration after a delay
    GetWorld()->GetTimerManager().SetTimer(RegenTimerHandle, this, &ADummy::RegenHealth, RegenDelay, false);
}

void ADummy::RegenHealth() {
    // Restore health fully
    Health = MaxHealth;
    // Update health bar UI to show full health again
    HealthBarWidget->UpdateHealthOnChange(Health, MaxHealth);
}
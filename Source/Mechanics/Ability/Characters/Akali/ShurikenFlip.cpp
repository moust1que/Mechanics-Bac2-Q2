#include "ShurikenFlip.h"
#include "../../../Character/BaseCharacter.h"
#include "../../../Character/Characters/Akali.h"
#include "ShurikenProjectile.h"

UShurikenFlip::UShurikenFlip() {
    Level = 0;
    AbilityID = 2;
    MaxLevel = 5;
    BaseCooldown = 16.0f;
    BaseRessourceCost = 30.0f;
}

void UShurikenFlip::UpdateStats() {
    TotBaseDamage = 70.0f * Level;
    Cooldown = BaseCooldown - 1.5f * (Level - 1);
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
    if(Level == 0) return;

    if(IsOnCooldown) return;

    if(CanRecast) {
        PerformRecast();
        return;
    }

    FVector SpawnLocation = CurCharacter->GetActorLocation() + CurCharacter-> GetActorForwardVector() * 100.0f;
    FRotator SpawnRotation = CurCharacter->GetActorRotation();

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = CurCharacter;

    AShurikenProjectile* Projectile = CurCharacter->GetWorld()->SpawnActor<AShurikenProjectile>(Cast<AAkali>(CurCharacter)->ShurikenProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);

    if(Projectile) {
        Projectile->OnShurikenHit.BindUObject(this, &UShurikenFlip::OnShurikenHit);
    }

    StartCooldown();
}

void UShurikenFlip::OnShurikenHit(AActor* HitActor, FVector HitLocation) {
    CanRecast = true;
    RecastLocation = HitLocation;
    RecastTarget = HitActor;

    CurCharacter->GetWorld()->GetTimerManager().SetTimer(RecastWindowTimer, this, &UShurikenFlip::CancelRecast, MarkTimer, false);
}

void UShurikenFlip::PerformRecast() {
    CanRecast = false;
    CurCharacter->GetWorld()->GetTimerManager().ClearTimer(RecastWindowTimer);

    FVector DashTarget = RecastTarget ? RecastTarget->GetActorLocation() : RecastLocation;

    FVector DashDirection = (DashTarget - CurCharacter->GetActorLocation()).GetSafeNormal();
    float DashSpeed = 1200.0f;

    CurCharacter->LaunchCharacter(DashDirection * DashSpeed, true, true);
}

void UShurikenFlip::CancelRecast() {
    CanRecast = false;
    RecastTarget = nullptr;
}

void UShurikenFlip::ResetCooldown() {
    UE_LOG(LogTemp, Warning, TEXT("Reset Cooldown"));
    IsOnCooldown = false;
}

void UShurikenFlip::StartCooldown() {
    IsOnCooldown = true;
    CurCharacter->GetWorld()->GetTimerManager().SetTimer(CooldownTimer, this, &UShurikenFlip::ResetCooldown, Cooldown, false);

    CurCharacter->HUDWidget->StartCooldown(this);
}
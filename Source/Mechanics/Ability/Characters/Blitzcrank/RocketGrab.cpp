#include "RocketGrab.h"
#include "RocketGrabProjectile.h"

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
    if(Level == 0 || IsOnCooldown) return;

    StartCastTimer(0.25f, "LaunchAttack");

    CurCharacter->CanMove = false;
}

void URocketGrab::LaunchAttack() {
    CurCharacter->IsUsingAbility = false;
    CurCharacter->OnAbilityOverlayHideRequested();
    CurCharacter->Ressource -= RessourceCost;
    CurCharacter->HUDWidget->UpdateResourceOnChange();
    CurCharacter->HealthBarWidget->UpdateResourceOnChange(CurCharacter->Ressource, CurCharacter->MaxRessource);

    FVector SpawnLocation = CurCharacter->GetActorLocation() + CurCharacter->GetActorForwardVector() * 100.0f + FVector(0.0f, 0.0f, 30.0f);
    FRotator SpawnRotation = CurCharacter->GetActorRotation();

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = CurCharacter;

    ARocketGrabProjectile* Grapple = CurCharacter->GetWorld()->SpawnActor<ARocketGrabProjectile>(BlitzcrankCharacter->RocketGrabProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);

    if(Grapple) {
        Grapple->OnGrappleHit.BindUObject(this, &URocketGrab::OnGrappleHit);
    }

    StartCooldown(Cooldown, true);
}

void URocketGrab::OnGrappleHit(AActor* HitActor, FVector HitLocation) {
    ABaseCharacter* HitCharacter = Cast<ABaseCharacter>(HitActor);
    if(HitCharacter) {
        HitCharacter->ReceiveDamage(TotalDamage);
    }
}

TArray<float> URocketGrab::GetArguments() {
    UpdateStats();
    return { Arg1, Arg2, Arg3 };
}
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
    // Cast the current character to Blitzcrank-specific class for ability power access
    BlitzcrankCharacter = Cast<ABlitzcrank>(CurCharacter);

    // Update cooldown scaling with level
    Cooldown = BaseCooldown - 1.0f * (Level - 1);
    // Resource cost stays constant here but can be updated similarly if needed
    RessourceCost = BaseRessourceCost;
    // Calculate damage based on level and ability power scaling
    BaseDamage = 105.0f + 45.0f * Level;
    AbilityDamage = 0.0f;
    AbilityPower = BlitzcrankCharacter->AbilityPower * 1.2f;
    TotalDamage = BaseDamage + AbilityDamage + AbilityPower;

    Arg1 = TotalDamage;
}

void URocketGrab::ActivateAbility() {
    if(Level == 0 || IsOnCooldown) return;

    // Start a short cast time, then launch the attack
    StartCastTimer(0.25f, "LaunchAttack");

    // Prevent movement during cast
    CurCharacter->CanMove = false;
}

void URocketGrab::LaunchAttack() {
    // Reset ability usage flags and UI
    CurCharacter->IsUsingAbility = false;
    CurCharacter->OnAbilityOverlayHideRequested();
    // Deduct resource cost and update UI
    CurCharacter->Ressource -= RessourceCost;
    CurCharacter->HUDWidget->UpdateResourceOnChange();
    CurCharacter->HealthBarWidget->UpdateResourceOnChange(CurCharacter->Ressource, CurCharacter->MaxRessource);

    // Calculate spawn position slightly in front and above the character transform
    FVector SpawnLocation = CurCharacter->GetActorLocation() + CurCharacter->GetActorForwardVector() * 100.0f + FVector(0.0f, 0.0f, 30.0f);
    FRotator SpawnRotation = CurCharacter->GetActorRotation();

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = CurCharacter;

    // Spawn the rocket grab projectile
    ARocketGrabProjectile* Grapple = CurCharacter->GetWorld()->SpawnActor<ARocketGrabProjectile>(BlitzcrankCharacter->RocketGrabProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);

    if(Grapple) {
        // Bind hit event delegate to handle damage dealing
        Grapple->OnGrappleHit.BindUObject(this, &URocketGrab::OnGrappleHit);
    }

    // Start cooldown timer
    StartCooldown(Cooldown, true);
}

void URocketGrab::OnGrappleHit(AActor* HitActor, FVector HitLocation) {
    // If we hit a character, apply damage
    ABaseCharacter* HitCharacter = Cast<ABaseCharacter>(HitActor);
    if(HitCharacter) {
        HitCharacter->ReceiveDamage(TotalDamage);
    }
}

// Return relevant float arguments for the tooltip
TArray<float> URocketGrab::GetArguments() {
    UpdateStats();
    return { Arg1, Arg2, Arg3 };
}
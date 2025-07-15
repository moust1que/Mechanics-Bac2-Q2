#include "ShurikenFlip.h"
#include "../../../Character/BaseCharacter.h"
#include "ShurikenProjectile.h"
#include "Components/CapsuleComponent.h"

// Constructor: Initializes default values for the ability
UShurikenFlip::UShurikenFlip() {
    Level = 0;
    AbilityID = 2;
    MaxLevel = 5;
    BaseCooldown = 16.0f;
    BaseRessourceCost = 30.0f;
}

// Updates damage and cooldown based on level and character stats
void UShurikenFlip::UpdateStats() {
    AkaliCharacter = Cast<AAkali>(CurCharacter);
    
    // Calculate total base damage and cooldown reduction by level
    TotBaseDamage = Level == 0 ? 70.0f : 70.0f * Level;
    Cooldown = BaseCooldown - 1.5f * (Level - 1);
    RessourceCost = BaseRessourceCost;
    // Calculate damage components for initial attack
    BaseDamage = TotBaseDamage * 0.3f;
    AbilityDamage = AkaliCharacter->AbilityDamage * 0.3f;
    AbilityPower = AkaliCharacter->AbilityPower * 0.33f;
    TotalDamage = BaseDamage + AbilityDamage + AbilityPower;
    
    // Calculate damage components for recast attack
    RecastBaseDamage = TotBaseDamage * 0.7f;
    RecastAbilityDamage = AkaliCharacter->AbilityDamage * 0.7f;
    RecastAbilityPower = AkaliCharacter->AbilityPower * 0.77f;
    RecastTotalDamage = RecastBaseDamage + RecastAbilityDamage + RecastAbilityPower;

    // Store damage args for the tooltip
    Arg1 = TotalDamage;
    Arg2 = RecastTotalDamage;
}

// Activates the ability. Triggers recast or initial dash + projectile
void UShurikenFlip::ActivateAbility() {
    if(Level == 0 || IsOnCooldown) return;

    if(CanRecast) {
        StartCastTimer(0.25f, "PerformRecast"); // Schedule recast action shortly after activation
        return;
    }

    StartCastTimer(0.4f, "LaunchAttack"); // Schedule initial attack launch
}

// Perform backward dash and spawn shuriken projectile
void UShurikenFlip::LaunchAttack() {
    // Hide ability UI overlays and consume resources
    CurCharacter->OnAbilityOverlayHideRequested();
    CurCharacter->Ressource -= RessourceCost;
    CurCharacter->HUDWidget->UpdateResourceOnChange();
    CurCharacter->HealthBarWidget->UpdateResourceOnChange(CurCharacter->Ressource, CurCharacter->MaxRessource);
    // Disable collision during dash to prevent physics interference
    CurCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // Setup dash parameters. Move backward 400 units over 0.5s
    FVector BackwardDirection = -CurCharacter->GetActorForwardVector();
    float BackDashDistance = 400.0f;

    DashStartLocation = CurCharacter->GetActorLocation();
    DashTargetLocation = DashStartLocation + BackwardDirection * BackDashDistance;
    DashDuration = 0.5f;
    DashElapsedTime = 0.0f;

    // Start dash movement timer
    CurCharacter->GetWorld()->GetTimerManager().SetTimer(DashTimerHandle, this, &UShurikenFlip::HandleDashTick, 0.01f, true);

    // Spawn shuriken projectile slightly ahead and above character
    FVector SpawnLocation = CurCharacter->GetActorLocation() + CurCharacter-> GetActorForwardVector() * 100.0f + FVector(0.0f, 0.0f, 20.0f);
    FRotator SpawnRotation = CurCharacter->GetActorRotation();

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = CurCharacter;

    AShurikenProjectile* Projectile = CurCharacter->GetWorld()->SpawnActor<AShurikenProjectile>(AkaliCharacter->ShurikenProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);

    if(Projectile) {
        // Bind hit and miss events from projectile
        Projectile->OnShurikenHit.BindUObject(this, &UShurikenFlip::OnShurikenHit);
        Projectile->OnShurikenNoHit.BindUObject(this, &UShurikenFlip::OnShurikenMiss);
    }

    CurCharacter->IsUsingAbility = false;
    
    // Start cooldown timer
    StartCooldown(Cooldown, false);
}

// Handles character movement during dash, checking for collision with external walls
void UShurikenFlip::HandleDashTick() {
    DashElapsedTime += GetWorld()->GetDeltaSeconds();
    float Alpha = FMath::Clamp(DashElapsedTime / DashDuration, 0.0f, 1.0f);

    FVector NewLocation = FMath::Lerp(DashStartLocation, DashTargetLocation, Alpha);
    NewLocation.Z = CurCharacter->GetActorLocation().Z; // Maintain Z height
    FRotator OldRotation = CurCharacter->GetActorRotation();

    // Collision check for external walls between current and new location
    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(CurCharacter);

    bool HitWall = GetWorld()->SweepSingleByChannel(Hit, CurCharacter->GetActorLocation(), NewLocation, FQuat::Identity, ECC_WorldStatic, FCollisionShape::MakeCapsule(CurCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius(), CurCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()), Params);

    if(HitWall && Hit.GetActor()->ActorHasTag("Walls")) {
        // Re-enable collision and stop dash if external wall is hit
        CurCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        GetWorld()->GetTimerManager().ClearTimer(DashTimerHandle);
        return;
    }

    // Move character smoothly and keep rotation unchanged
    CurCharacter->SetActorLocation(NewLocation, true);
    CurCharacter->SetActorRotation(OldRotation);

    if(Alpha >= 1.0f) {
        // End dash and re-enable collision
        CurCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        GetWorld()->GetTimerManager().ClearTimer(DashTimerHandle);
        CurCharacter->IsUsingAbility = false;
    }
}

// Called when shuriken hits an actor. Apply damage and enable recast window
void UShurikenFlip::OnShurikenHit(AActor* HitActor, FVector HitLocation) {
    ResetCooldown();
    CanRecast = true;
    CurCharacter->HUDWidget->UpdateSpellRecastDisplay(this);
    RecastLocation = HitLocation;
    RecastTarget = HitActor;

    ABaseCharacter* HitCharacter = Cast<ABaseCharacter>(HitActor);
    if(HitCharacter) {
        HitCharacter->ReceiveDamage(TotalDamage); // Deal damage to hit character
    }

    // Start timer to allow recast window before cancelling
    CurCharacter->GetWorld()->GetTimerManager().SetTimer(RecastWindowTimer, this, &UShurikenFlip::CancelRecast, MarkTimer, false);
}

// Called if shuriken misses. Reset cooldown and start it normally
void UShurikenFlip::OnShurikenMiss() {
    ResetCooldown();

    StartCooldown(Cooldown, true);
}

// Perform the recast dash towards hit location or last known location
void UShurikenFlip::PerformRecast() {
    CurCharacter->OnAbilityOverlayHideRequested();
    CurCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    CanRecast = false;
    CurCharacter->HUDWidget->UpdateSpellRecastDisplay(this);
    CurCharacter->GetWorld()->GetTimerManager().ClearTimer(RecastWindowTimer);

    FVector DashTarget;
    if(RecastTarget) {
        UCapsuleComponent* TargetCapsuleComponent = RecastTarget->FindComponentByClass<UCapsuleComponent>();
        UCapsuleComponent* MyCapsuleComponent = CurCharacter->GetCapsuleComponent();

        // If both character and target have capsules, position dash target slightly before target
        if(TargetCapsuleComponent && MyCapsuleComponent) {
            FVector Direction = (RecastTarget->GetActorLocation() - CurCharacter->GetActorLocation()).GetSafeNormal();
            DashTarget = RecastTarget->GetActorLocation() - Direction * 8.0f;;
        }else {
            DashTarget = RecastLocation;
        }
    }

    // Initialize recast dash movement parameters
    RecastDashStartLocation = CurCharacter->GetActorLocation();
    RecastDashTargetLocation = DashTarget;
    RecastDashElapsedTime = 0.0f;

    float Distance = FVector::Dist(RecastDashStartLocation, RecastDashTargetLocation);

    float DashSpeed = 1200.0f;

    RecastDashDuration = Distance / DashSpeed;

    // Start timer to update recast dash movement
    CurCharacter->GetWorld()->GetTimerManager().SetTimer(RecastDashTimerHandle, this, &UShurikenFlip::HandleRecastDashTick, 0.01f, true);

    CurCharacter->IsUsingAbility = false;

    // Start cooldown after recast
    StartCooldown(Cooldown, true);
}

// Handles movement and damage application for recast dash
void UShurikenFlip::HandleRecastDashTick() {
    RecastDashElapsedTime += GetWorld()->GetDeltaSeconds();
    float Alpha = FMath::Clamp(RecastDashElapsedTime / RecastDashDuration, 0.0f, 1.0f);

    FVector NewLocation = FMath::Lerp(RecastDashStartLocation, RecastDashTargetLocation, Alpha);
    NewLocation.Z = CurCharacter->GetActorLocation().Z; // Keep same height
    FRotator OldRotation = CurCharacter->GetActorRotation();
    CurCharacter->SetActorLocation(NewLocation, true);
    CurCharacter->SetActorRotation(OldRotation);

    if(Alpha >= 1.0f) {
        // End recast dash, re-enable collision and deal damage
        CurCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

        CurCharacter->GetWorld()->GetTimerManager().ClearTimer(RecastDashTimerHandle);

        ABaseCharacter* HitCharacter = Cast<ABaseCharacter>(RecastTarget);
        if(HitCharacter) {
            HitCharacter->ReceiveDamage(RecastTotalDamage);
        }
        CurCharacter->IsUsingAbility = false;
    }
}

// Cancel recast state and start cooldown
void UShurikenFlip::CancelRecast() {
    CurCharacter->IsUsingAbility = false;
    CanRecast = false;
    CurCharacter->HUDWidget->UpdateSpellRecastDisplay(this);
    CurCharacter->CancelAttack();

    StartCooldown(Cooldown, true);
}

// Return ability damage arguments, updating stats first
TArray<float> UShurikenFlip::GetArguments() {
    UpdateStats();
    return { Arg1, Arg2, Arg3 };
}
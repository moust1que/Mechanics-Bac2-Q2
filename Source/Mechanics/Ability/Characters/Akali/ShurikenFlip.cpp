#include "ShurikenFlip.h"
#include "../../../Character/BaseCharacter.h"
#include "ShurikenProjectile.h"
#include "Components/CapsuleComponent.h"

UShurikenFlip::UShurikenFlip() {
    Level = 0;
    AbilityID = 2;
    MaxLevel = 5;
    BaseCooldown = 16.0f;
    BaseRessourceCost = 30.0f;
}

void UShurikenFlip::UpdateStats() {
    AkaliCharacter = Cast<AAkali>(CurCharacter);
    
    TotBaseDamage = Level == 0 ? 70.0f : 70.0f * Level;
    Cooldown = BaseCooldown - 1.5f * (Level - 1);
    RessourceCost = BaseRessourceCost;
    BaseDamage = TotBaseDamage * 0.3f;
    AbilityDamage = AkaliCharacter->AbilityDamage * 0.3f;
    AbilityPower = AkaliCharacter->AbilityPower * 0.33f;
    TotalDamage = BaseDamage + AbilityDamage + AbilityPower;
    
    RecastBaseDamage = TotBaseDamage * 0.7f;
    RecastAbilityDamage = AkaliCharacter->AbilityDamage * 0.7f;
    RecastAbilityPower = AkaliCharacter->AbilityPower * 0.77f;
    RecastTotalDamage = RecastBaseDamage + RecastAbilityDamage + RecastAbilityPower;

    Arg1 = TotalDamage;
    Arg2 = RecastTotalDamage;
}

void UShurikenFlip::ActivateAbility() {
    if(Level == 0 || IsOnCooldown) return;

    if(CanRecast) {
        StartCastTimer(0.25f, "PerformRecast");
        return;
    }

    StartCastTimer(0.4f, "LaunchAttack");
}

void UShurikenFlip::LaunchAttack() {
    CurCharacter->IsUsingAbility = false;
    CurCharacter->Ressource -= RessourceCost;
    CurCharacter->HUDWidget->UpdateResourceOnChange();
    CurCharacter->HealthBarWidget->UpdateResourceOnChange(CurCharacter->Ressource, CurCharacter->MaxRessource);
    CurCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    FVector BackwardDirection = -CurCharacter->GetActorForwardVector();
    float BackDashDistance = 400.0f;

    DashStartLocation = CurCharacter->GetActorLocation();
    DashTargetLocation = DashStartLocation + BackwardDirection * BackDashDistance;
    DashDuration = 0.5f;
    DashElapsedTime = 0.0f;

    CurCharacter->GetWorld()->GetTimerManager().SetTimer(DashTimerHandle, this, &UShurikenFlip::HandleDashTick, 0.01f, true);

    FVector SpawnLocation = CurCharacter->GetActorLocation() + CurCharacter-> GetActorForwardVector() * 100.0f + FVector(0.0f, 0.0f, 20.0f);
    FRotator SpawnRotation = CurCharacter->GetActorRotation();

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = CurCharacter;

    AShurikenProjectile* Projectile = CurCharacter->GetWorld()->SpawnActor<AShurikenProjectile>(Cast<AAkali>(CurCharacter)->ShurikenProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);

    if(Projectile) {
        Projectile->OnShurikenHit.BindUObject(this, &UShurikenFlip::OnShurikenHit);
        Projectile->OnShurikenNoHit.BindUObject(this, &UShurikenFlip::OnShurikenMiss);
    }
    
    StartCooldown(Cooldown);
}

void UShurikenFlip::HandleDashTick() {
    DashElapsedTime += GetWorld()->GetDeltaSeconds();
    float Alpha = FMath::Clamp(DashElapsedTime / DashDuration, 0.0f, 1.0f);

    FVector NewLocation = FMath::Lerp(DashStartLocation, DashTargetLocation, Alpha);
    NewLocation.Z = CurCharacter->GetActorLocation().Z;
    FRotator OldRotation = CurCharacter->GetActorRotation();

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(CurCharacter);

    bool HitWall = GetWorld()->SweepSingleByChannel(Hit, CurCharacter->GetActorLocation(), NewLocation, FQuat::Identity, ECC_WorldStatic, FCollisionShape::MakeCapsule(CurCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius(), CurCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()), Params);

    if(HitWall && Hit.GetActor()->ActorHasTag("Walls")) {
        CurCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        GetWorld()->GetTimerManager().ClearTimer(DashTimerHandle);
        return;
    }

    CurCharacter->SetActorLocation(NewLocation, true);
    CurCharacter->SetActorRotation(OldRotation);

    if(Alpha >= 1.0f) {
        CurCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        GetWorld()->GetTimerManager().ClearTimer(DashTimerHandle);
    }
}

void UShurikenFlip::OnShurikenHit(AActor* HitActor, FVector HitLocation) {
    ResetCooldown();
    CanRecast = true;
    CurCharacter->HUDWidget->UpdateSpellRecastDisplay(this);
    RecastLocation = HitLocation;
    RecastTarget = HitActor;

    ABaseCharacter* HitCharacter = Cast<ABaseCharacter>(HitActor);
    if(HitCharacter) {
        HitCharacter->ReceiveDamage(TotalDamage);
    }

    CurCharacter->GetWorld()->GetTimerManager().SetTimer(RecastWindowTimer, this, &UShurikenFlip::CancelRecast, MarkTimer, false);
}

void UShurikenFlip::OnShurikenMiss() {
    ResetCooldown();

    StartCooldown(Cooldown);
}

void UShurikenFlip::PerformRecast() {
    CurCharacter->IsUsingAbility = false;
    CurCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    CanRecast = false;
    CurCharacter->HUDWidget->UpdateSpellRecastDisplay(this);
    CurCharacter->GetWorld()->GetTimerManager().ClearTimer(RecastWindowTimer);

    FVector DashTarget;
    if(RecastTarget) {
        UCapsuleComponent* TargetCapsuleComponent = RecastTarget->FindComponentByClass<UCapsuleComponent>();
        UCapsuleComponent* MyCapsuleComponent = CurCharacter->GetCapsuleComponent();

        if(TargetCapsuleComponent && MyCapsuleComponent) {
            float CombinedRadius = TargetCapsuleComponent->GetScaledCapsuleRadius() + MyCapsuleComponent->GetScaledCapsuleRadius();
            FVector Direction = (RecastTarget->GetActorLocation() - CurCharacter->GetActorLocation()).GetSafeNormal();
            DashTarget = RecastTarget->GetActorLocation() - Direction * CombinedRadius;
        }else {
            DashTarget = RecastLocation;
        }
    }

    RecastDashStartLocation = CurCharacter->GetActorLocation();
    RecastDashTargetLocation = DashTarget;
    RecastDashElapsedTime = 0.0f;

    float Distance = FVector::Dist(RecastDashStartLocation, RecastDashTargetLocation);

    float DashSpeed = 1200.0f;

    RecastDashDuration = Distance / DashSpeed;

    CurCharacter->GetWorld()->GetTimerManager().SetTimer(RecastDashTimerHandle, this, &UShurikenFlip::HandleRecastDashTick, 0.01f, true);

    StartCooldown(Cooldown);
}

void UShurikenFlip::HandleRecastDashTick() {
    RecastDashElapsedTime += CurCharacter->GetWorld()->GetDeltaSeconds();
    float Alpha = FMath::Clamp(RecastDashElapsedTime / RecastDashDuration, 0.0f, 1.0f);

    FVector NewLocation = FMath::Lerp(RecastDashStartLocation, RecastDashTargetLocation, Alpha);
    NewLocation.Z = CurCharacter->GetActorLocation().Z;
    FRotator OldRotation = CurCharacter->GetActorRotation();
    CurCharacter->SetActorLocation(NewLocation, true);
    CurCharacter->SetActorRotation(OldRotation);

    if(Alpha >= 1.0f) {
        CurCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

        CurCharacter->GetWorld()->GetTimerManager().ClearTimer(RecastDashTimerHandle);

        ABaseCharacter* HitCharacter = Cast<ABaseCharacter>(RecastTarget);
        if(HitCharacter) {
            HitCharacter->ReceiveDamage(RecastTotalDamage);
        }
    }
}

void UShurikenFlip::CancelRecast() {
    CanRecast = false;
    CurCharacter->HUDWidget->UpdateSpellRecastDisplay(this);

    StartCooldown(Cooldown);
}

TArray<float> UShurikenFlip::GetArguments() {
    UpdateStats();
    return { Arg1, Arg2, Arg3 };
}
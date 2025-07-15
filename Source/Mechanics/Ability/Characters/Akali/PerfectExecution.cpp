#include "PerfectExecution.h"
#include "Components/CapsuleComponent.h"

// Constructor to initialize default values for the ability
UPerfectExecution::UPerfectExecution() {
    Level = 0;
    AbilityID = 3;
    MaxLevel = 3;
    BaseCooldown = 120.0f;
    BaseRessourceCost = 0.0f;
}

// Updates ability stats based on current level and character stats
void UPerfectExecution::UpdateStats() {
    AkaliCharacter = Cast<AAkali>(CurCharacter);
    
    // Basic damage scaling based on ability level
    MinBaseDamage = Level == 0 ? 70.0f : 70.0f * Level;
    MaxBaseDamage = Level == 0 ? 210.0f : 210.0f * Level;
    Cooldown = BaseCooldown - 30.0f * (Level - 1);
    RessourceCost = BaseRessourceCost;
    // First cast damage calculation
    BaseDamage = Level == 0 ? 110.0f : 110.0f * Level;
    AbilityDamage = 0.0f;
    AbilityPower = AkaliCharacter->AbilityPower * 0.3f;
    TotalDamage = BaseDamage + AbilityDamage + AbilityPower;
    
    // Recast base damage placeholder value
    RecastBaseDamage = MinBaseDamage + ((MinBaseDamage - MaxBaseDamage) / 0.7f) * (100 / 100 - 1);
    RecastAbilityDamage = 0.0f;
    RecastAbilityPower = AkaliCharacter->AbilityPower * 0.9f;
    RecastTotalDamage = RecastBaseDamage + RecastAbilityDamage + RecastAbilityPower;

    // Store arguments for tooltip
    Arg1 = TotalDamage;
    Arg2 = MinBaseDamage;
    Arg3 = MaxBaseDamage;

    NeedEnemyTarget = true;

    AbilityRange = 675.0f;
}

// Called to activate the ability
void UPerfectExecution::ActivateAbility() {
    if(Level == 0 || IsOnCooldown) return;

    CurCharacter->IsUsingAbility = false;
    CurCharacter->OnAbilityOverlayHideRequested();
    DamagedEnemies.Empty(); // Reset hit tracking

    // If the ability is in recast phase, perform second dash
    if(CanRecast) {
        PerformRecast();
        return;
    }

    // Apply resource cost and update UI
    CurCharacter->Ressource -= RessourceCost;
    CurCharacter->HUDWidget->UpdateResourceOnChange();
    CurCharacter->HealthBarWidget->UpdateResourceOnChange(CurCharacter->Ressource, CurCharacter->MaxRessource);

    // Calculate dash target location
    DashStartLocation = CurCharacter->GetActorLocation();
    FVector DirectionToEnemy = (EnemyTarget->GetActorLocation() - DashStartLocation).GetSafeNormal();
    DashTargetLocation = EnemyTarget->GetActorLocation() + DirectionToEnemy * 150.0f;

    // Calculate dash duration based on speed
    float DashSpeed = 1500.0f;
    float DashDistance = FVector::Dist(DashStartLocation, DashTargetLocation);
    DashDuration = DashDistance / DashSpeed;
    DashElapsedTime = 0.0f;

    // Disable collision during dash
    CurCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    // Start dash tick timer
    CurCharacter->GetWorld()->GetTimerManager().SetTimer(DashTimerHandle, this, &UPerfectExecution::HandleDashTick, 0.01f, true);

    // Prepare recast activation after 2.5 seconds
    FTimerHandle RecastEnableTimerHandle;
    StartCooldown(2.5f, false);
    CurCharacter->GetWorld()->GetTimerManager().SetTimer(
        RecastEnableTimerHandle,
        [this]() {
            CanRecast = true;
            NeedEnemyTarget = false;
            EnemyTarget = nullptr;
            CurCharacter->HUDWidget->UpdateSpellRecastDisplay(this);
            CurCharacter->GetWorld()->GetTimerManager().SetTimer(RecastWindowTimer, this, &UPerfectExecution::CancelRecast, 10.0f, false);
        },
        2.5f,
        false
    );
}

// Handles movement and collision detection during the first dash
void UPerfectExecution::HandleDashTick() {
    DashElapsedTime += CurCharacter->GetWorld()->GetDeltaSeconds();
    float Alpha = FMath::Clamp(DashElapsedTime / DashDuration, 0.0f, 1.0f);

    FVector PreviousLocation = CurCharacter->GetActorLocation();
    FVector NewLocation = FMath::Lerp(DashStartLocation, DashTargetLocation, Alpha);
    NewLocation.Z = PreviousLocation.Z;
    FRotator OldRotation = CurCharacter->GetActorRotation();

    // Sweep for enemies hit during the dash
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(CurCharacter);

    TArray<FHitResult> HitResults;
    bool Hit = CurCharacter->GetWorld()->SweepMultiByChannel(
        HitResults,
        PreviousLocation,
        NewLocation,
        FQuat::Identity,
        ECC_Pawn,
        FCollisionShape::MakeCapsule(34.0f, 88.0f),
        Params
    );

    // Apply damage to any valid enemies
    if(Hit) {
        for(const FHitResult& HitResult : HitResults) {
            if(ABaseCharacter* HitCharacter = Cast<ABaseCharacter>(HitResult.GetActor())) {
                if(!DamagedEnemies.Contains(HitCharacter)) {
                    DamagedEnemies.Add(HitCharacter);
                    HitCharacter->ReceiveDamage(TotalDamage);
                }
            }
        } 
    }

    // Move character and restore collision when done
    CurCharacter->SetActorLocation(NewLocation, true);
    CurCharacter->SetActorRotation(OldRotation);

    if(Alpha >= 1.0f) {
        CurCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        CurCharacter->GetWorld()->GetTimerManager().ClearTimer(DashTimerHandle);
    }
}

// Executes the second dash toward mouse cursor location
void UPerfectExecution::PerformRecast() {
    CurCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    CanRecast = false;
    NeedEnemyTarget = true;
    CurCharacter->HUDWidget->UpdateSpellRecastDisplay(this);
    CurCharacter->GetWorld()->GetTimerManager().ClearTimer(RecastWindowTimer);

    RecastStartLocation = CurCharacter->GetActorLocation();

    // Get world location from mouse cursor
    FVector WorldMouseLocation;
    FVector WorldMouseDirection;

    APlayerController* PlayerController = Cast<APlayerController>(CurCharacter->GetController());
    if(PlayerController->DeprojectMousePositionToWorld(WorldMouseLocation, WorldMouseDirection)) {
        FVector Start = WorldMouseLocation;
        FVector End = Start + WorldMouseDirection * 10000.0f;

        FHitResult HitResult;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(CurCharacter);

        if(CurCharacter->GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params)) {
            FVector Direction = (HitResult.ImpactPoint - RecastStartLocation).GetSafeNormal();
            RecastTargetLocation = RecastStartLocation + Direction * 800.0f;
        }
    }
    RecastElapsedTime = 0.0f;

    float Distance = FVector::Dist(RecastStartLocation, RecastTargetLocation);
    float DashSpeed = 3000.0f;
    RecastDuration = Distance / DashSpeed;

    CurCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    // Start the recast dash tick
    CurCharacter->GetWorld()->GetTimerManager().SetTimer(RecastTimerHandle, this, &UPerfectExecution::HandleRecastDashTick, 0.01f, true);

    StartCooldown(Cooldown, true);
}

// Handles recast dash movement and collision
void UPerfectExecution::HandleRecastDashTick() {
    RecastElapsedTime += CurCharacter->GetWorld()->GetDeltaSeconds();
    float Alpha = FMath::Clamp(RecastElapsedTime / RecastDuration, 0.0f, 1.0f);

    FVector PreviousLocation = CurCharacter->GetActorLocation();
    FVector NewLocation = FMath::Lerp(RecastStartLocation, RecastTargetLocation, Alpha);
    NewLocation.Z = PreviousLocation.Z;
    FRotator OldRotation = CurCharacter->GetActorRotation();

    // Sweep for enemies hit during the recast dash
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(CurCharacter);

    TArray<FHitResult> HitResults;
    bool Hit = CurCharacter->GetWorld()->SweepMultiByChannel(
        HitResults,
        PreviousLocation,
        NewLocation,
        FQuat::Identity,
        ECC_Pawn,
        FCollisionShape::MakeCapsule(34.0f, 88.0f),
        Params
    );

    // Recalculate and apply damage based on enemy health
    if(Hit) {
        for(const FHitResult& HitResult : HitResults) {
            if(ABaseCharacter* HitCharacter = Cast<ABaseCharacter>(HitResult.GetActor())) {
                if(!DamagedEnemies.Contains(HitCharacter)) {
                    DamagedEnemies.Add(HitCharacter);

                    RecastBaseDamage = MinBaseDamage + ((MinBaseDamage - MaxBaseDamage) / 0.7f) * (HitCharacter->Health / HitCharacter->MaxHealth - 1);
                    RecastTotalDamage = RecastBaseDamage + RecastAbilityDamage + RecastAbilityPower;
                    HitCharacter->ReceiveDamage(RecastTotalDamage);
                }
            }
        } 
    }

    // Move character and end dash when finished
    CurCharacter->SetActorLocation(NewLocation, true);
    CurCharacter->SetActorRotation(OldRotation);

    if(Alpha >= 1.0f) {
        CurCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        CurCharacter->GetWorld()->GetTimerManager().ClearTimer(RecastTimerHandle);
    }
}

// Cancels the ability recast if time window expires
void UPerfectExecution::CancelRecast() {
    CanRecast = false;
    NeedEnemyTarget = true;  
    CurCharacter->HUDWidget->UpdateSpellRecastDisplay(this);
    CurCharacter->CancelAttack();

    StartCooldown(Cooldown, true);
}

// Returns current arguments used for tooltip
TArray<float> UPerfectExecution::GetArguments() {
    UpdateStats();
    return { Arg1, Arg2, Arg3 };
}
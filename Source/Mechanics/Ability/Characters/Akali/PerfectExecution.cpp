#include "PerfectExecution.h"
#include "Components/CapsuleComponent.h"

UPerfectExecution::UPerfectExecution() {
    Level = 0;
    AbilityID = 3;
    MaxLevel = 3;
    BaseCooldown = 120.0f;
    BaseRessourceCost = 0.0f;
}

void UPerfectExecution::UpdateStats() {
    AkaliCharacter = Cast<AAkali>(CurCharacter);
    
    MinBaseDamage = Level == 0 ? 70.0f : 70.0f * Level;
    MaxBaseDamage = Level == 0 ? 210.0f : 210.0f * Level;
    Cooldown = BaseCooldown - 30.0f * (Level - 1);
    RessourceCost = BaseRessourceCost;
    BaseDamage = Level == 0 ? 110.0f : 110.0f * Level;
    AbilityDamage = 0.0f;
    AbilityPower = AkaliCharacter->AbilityPower * 0.3f;
    TotalDamage = BaseDamage + AbilityDamage + AbilityPower;
    
    RecastBaseDamage = MinBaseDamage + ((MinBaseDamage - MaxBaseDamage) / 0.7f) * (100 / 100 - 1);
    RecastAbilityDamage = 0.0f;
    RecastAbilityPower = AkaliCharacter->AbilityPower * 0.9f;
    RecastTotalDamage = RecastBaseDamage + RecastAbilityDamage + RecastAbilityPower;

    Arg1 = TotalDamage;
    Arg2 = MinBaseDamage;
    Arg3 = MaxBaseDamage;

    NeedEnemyTarget = true;

    AbilityRange = 675.0f;
}

void UPerfectExecution::ActivateAbility() {
    if(Level == 0 || IsOnCooldown) return;

    CurCharacter->IsUsingAbility = false;
    DamagedEnemies.Empty();

    if(CanRecast) {
        PerformRecast();
        return;
    }
    CurCharacter->Ressource -= RessourceCost;
    CurCharacter->HUDWidget->UpdateResourceOnChange();
    CurCharacter->HealthBarWidget->UpdateResourceOnChange(CurCharacter->Ressource, CurCharacter->MaxRessource);

    DashStartLocation = CurCharacter->GetActorLocation();
    FVector DirectionToEnemy = (EnemyTarget->GetActorLocation() - DashStartLocation).GetSafeNormal();
    DashTargetLocation = EnemyTarget->GetActorLocation() + DirectionToEnemy * 150.0f;
    float DashSpeed = 1500.0f;
    float DashDistance = FVector::Dist(DashStartLocation, DashTargetLocation);

    DashDuration = DashDistance / DashSpeed;
    DashElapsedTime = 0.0f;

    CurCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    CurCharacter->GetWorld()->GetTimerManager().SetTimer(DashTimerHandle, this, &UPerfectExecution::HandleDashTick, 0.01f, true);

    FTimerHandle RecastEnableTimerHandle;
    StartCooldown(2.5f);
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

void UPerfectExecution::HandleDashTick() {
    DashElapsedTime += CurCharacter->GetWorld()->GetDeltaSeconds();
    float Alpha = FMath::Clamp(DashElapsedTime / DashDuration, 0.0f, 1.0f);

    FVector PreviousLocation = CurCharacter->GetActorLocation();
    FVector NewLocation = FMath::Lerp(DashStartLocation, DashTargetLocation, Alpha);
    NewLocation.Z = PreviousLocation.Z;
    FRotator OldRotation = CurCharacter->GetActorRotation();

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

    CurCharacter->SetActorLocation(NewLocation, true);
    CurCharacter->SetActorRotation(OldRotation);

    if(Alpha >= 1.0f) {
        CurCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        CurCharacter->GetWorld()->GetTimerManager().ClearTimer(DashTimerHandle);
    }
}

void UPerfectExecution::PerformRecast() {
    CurCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    CanRecast = false;
    NeedEnemyTarget = true;
    CurCharacter->HUDWidget->UpdateSpellRecastDisplay(this);
    CurCharacter->GetWorld()->GetTimerManager().ClearTimer(RecastWindowTimer);

    RecastStartLocation = CurCharacter->GetActorLocation();

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
    CurCharacter->GetWorld()->GetTimerManager().SetTimer(RecastTimerHandle, this, &UPerfectExecution::HandleRecastDashTick, 0.01f, true);

    StartCooldown(Cooldown);
}

void UPerfectExecution::HandleRecastDashTick() {
    RecastElapsedTime += CurCharacter->GetWorld()->GetDeltaSeconds();
    float Alpha = FMath::Clamp(RecastElapsedTime / RecastDuration, 0.0f, 1.0f);

    FVector PreviousLocation = CurCharacter->GetActorLocation();
    FVector NewLocation = FMath::Lerp(RecastStartLocation, RecastTargetLocation, Alpha);
    NewLocation.Z = PreviousLocation.Z;
    FRotator OldRotation = CurCharacter->GetActorRotation();

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

    CurCharacter->SetActorLocation(NewLocation, true);
    CurCharacter->SetActorRotation(OldRotation);

    if(Alpha >= 1.0f) {
        CurCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        CurCharacter->GetWorld()->GetTimerManager().ClearTimer(RecastTimerHandle);
    }
}

void UPerfectExecution::CancelRecast() {
    CanRecast = false;
    NeedEnemyTarget = true;  
    CurCharacter->HUDWidget->UpdateSpellRecastDisplay(this);
    StartCooldown(Cooldown);
}

TArray<float> UPerfectExecution::GetArguments() {
    UpdateStats();
    return { Arg1, Arg2, Arg3 };
}
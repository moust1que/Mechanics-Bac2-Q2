#include "AbilityTargetingIndicator.h"

void AAbilityTargetingIndicator::ShowIndicator(bool Show) {
    SetActorHiddenInGame(!Show);
    SetActorEnableCollision(false);
    SetActorTickEnabled(Show);
}
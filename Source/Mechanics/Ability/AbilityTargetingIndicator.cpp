#include "AbilityTargetingIndicator.h"

// Toggles the visibility and activity of the targeting indicator
void AAbilityTargetingIndicator::ShowIndicator(bool Show) {
    // Show or hide the actor in the game world
    SetActorHiddenInGame(!Show);
    // Disable collisions regardless of whether it's shown or not
    SetActorEnableCollision(false);
    // Enable ticking only if the indicator should be shown
    SetActorTickEnabled(Show);
}
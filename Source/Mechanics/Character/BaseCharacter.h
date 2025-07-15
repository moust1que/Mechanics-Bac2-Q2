#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Widget/MainHUD.h"
#include "../Widget/HealthBar.h"
#include "BaseCharacter.generated.h"

// Forward declaration
class UAbilityBase;
struct FInputActionInstance;
class USpringArmComponent;
class UCameraComponent;
class UNiagaraSystem;
class AAbilityTargetingIndicator;

// Ability input enum for mapping inputs to abilities
UENUM(BlueprintType) enum class EAbilityInputID : uint8 {
    None, A, Z, E, R
};

// Struct holding ability indicator actor classes for first and second cast visuals
USTRUCT(BlueprintType) struct FAbiliyIndicatorSet {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere) TSubclassOf<AActor> FirstCastIndicator;
    UPROPERTY(EditAnywhere) TSubclassOf<AActor> SecondCastIndicator;
};

UCLASS()
class MECHANICS_API ABaseCharacter : public ACharacter {
	GENERATED_BODY()

    public:
        ABaseCharacter();

        // Mapping from ability slot index to ability class
        UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Abilities) TMap<int, TSubclassOf<UAbilityBase>> AbilityMap;

        // Instantiated ability objects per slot
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Abilities) TMap<int, UAbilityBase*> InstantiatedAbilities;

        // Cursor effect particle system
        UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input) UNiagaraSystem* FXCursor;

        void HandleFunctionCall(FName functionName, const FInputActionInstance& Instance);

        void ActivateAbility(int AbilitySlot);

        // Camera zoom settings
        UPROPERTY(EditAnywhere, Category = Camera) float CameraZoomSpeed = 100.0f;
        UPROPERTY(EditAnywhere, Category = Camera) float CameraZoomMin = 1200.0f;
        UPROPERTY(EditAnywhere, Category = Camera) float CameraZoomMax = 2500.0f;

        // Widgets for UI display
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widget) UMainHUD* HUDWidget;
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widget) UHealthBar* HealthBarWidget;

        // If true, cooldowns will auto-refresh
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cooldowns) bool AutoRefreshCooldowns = false;

        // Is this character an enemy? Used for targeting logic
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Character) bool IsEnemy = false;

        // Type of resource used
        ERessourceType RessourceType;

        // Stats
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats) int Level;
        UPROPERTY(EditAnywhere, Category = Stats) int MaxLevel = 18;
        UPROPERTY(EditAnywhere, Category = Stats) float BaseHealth;
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats) float Health;
        UPROPERTY(EditAnywhere, Category = Stats) float HealthPerLevel;
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats) float MaxHealth;
        UPROPERTY(EditAnywhere, Category = Stats) float BaseRessource;
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats) float Ressource;
        UPROPERTY(EditAnywhere, Category = Stats) float RessourcePerLevel;
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats) float MaxRessource;
        UPROPERTY(EditAnywhere, Category = Stats) float BaseAbilityDamage;
        UPROPERTY(EditAnywhere, Category = Stats) float AbilityDamage;
        UPROPERTY(EditAnywhere, Category = Stats) float AbilityDamagePerLevel;
        UPROPERTY(EditAnywhere, Category = Stats) float BaseAbilityPower;
        UPROPERTY(EditAnywhere, Category = Stats) float AbilityPower;
        UPROPERTY(EditAnywhere, Category = Stats) float AbilityPowerPerLevel;
        UPROPERTY(EditAnywhere, Category = Stats) float BaseAttackSpeed;
        UPROPERTY(EditAnywhere, Category = Stats) float AttackSpeed;
        UPROPERTY(EditAnywhere, Category = Stats) float AttackSpeedRatio;
        UPROPERTY(EditAnywhere, Category = Stats) float BasePhysicalResist;
        UPROPERTY(EditAnywhere, Category = Stats) float PhysicalResist;
        UPROPERTY(EditAnywhere, Category = Stats) float PhysicalResistPerLevel;
        UPROPERTY(EditAnywhere, Category = Stats) float BaseMagicResist;
        UPROPERTY(EditAnywhere, Category = Stats) float MagicResist;
        UPROPERTY(EditAnywhere, Category = Stats) float MagicResistPerLevel;
        UPROPERTY(EditAnywhere, Category = Stats) float MoveSpeed;
        UPROPERTY(EditAnywhere, Category = Stats) float AttackRange;
        UPROPERTY(EditAnywhere, Category = Stats) float BaseHealthRegen;
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats) float HealthRegen;
        UPROPERTY(EditAnywhere, Category = Stats) float HealthRegenPerLevel;
        UPROPERTY(EditAnywhere, Category = Stats) float BaseRessourceRegen;
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats) float RessourceRegen;
        UPROPERTY(EditAnywhere, Category = Stats) float RessourceRegenPerLevel;

        // Ability indicators to show in world for targeting cues
        UPROPERTY(EditAnywhere, Category = Indicators) TMap<EAbilityInputID, FAbiliyIndicatorSet> AbilityIndicators;

        // Current active targeting indicator in the world
        UPROPERTY() AAbilityTargetingIndicator* CurrentTargetIndicator = nullptr;

        // Flags for ability and UI state
        bool IsUsingAbility = false;

        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Flag) bool HoverButton = false;

        UFUNCTION(BlueprintCallable, Category = Ability) void Ability1();
        UFUNCTION(BlueprintCallable, Category = Ability) void Ability2();
        UFUNCTION(BlueprintCallable, Category = Ability) void Ability3();
        UFUNCTION(BlueprintCallable, Category = Ability) void Ability4();

        UFUNCTION(BlueprintCallable, Category = Character) void LevelUP();
        virtual void ReceiveDamage(float Damage) {};

        UFUNCTION(BlueprintCallable, Category = Ability) void CancelAttack();
        UFUNCTION() bool IsInAbilityTargeting() const;

        virtual void Tick(float DeltaTime) override;

        void LaunchRegen();
        void HandleRegen();

        void OnAbilityOverlayHideRequested();

        // Controls if character can move
        bool CanMove = true;

    protected:
        virtual void BeginPlay() override;

    private:
        // Components for camera control
        USpringArmComponent* SpringArmComponent;
        UCameraComponent* CameraComponent;

        // Currently active ability input
        EAbilityInputID ActiveAbilityInputID = EAbilityInputID::None;

        // Cached movement destination
        FVector CachedDestination;

        bool WasCancellingAbility = false;

        // Used to measure input press duration for input logic
        float ShortPressThreshold = 0.3f;
        float FollowTime;

        // Rotation target and flags for smooth rotation on movement/aim
        FRotator TargetRotation;
        bool ShouldRotate = false;
        float RotationSpeed = 10.0f;

        // Target actor for abilities that require moving towards a target before use
        AActor* PendingAbilityTarget = nullptr;
        EAbilityInputID PendingAbilityInputID = EAbilityInputID::None;
        bool IsApproachingTarget = false;

        UFUNCTION() void OnSetDestinationStarted();
        UFUNCTION() void OnSetDestinationTriggered();
        UFUNCTION() void OnSetDestinationReleased();
        UFUNCTION() void ZoomCamera(const FInputActionInstance& Instance);
        UFUNCTION() void ConfirmAttack();
        UFUNCTION() void ActivateAttackMode(EAbilityInputID Ability);
        UFUNCTION() void OnAbilityOverlayRequested(EAbilityInputID Ability);
        UFUNCTION() void ExecuteAbility(EAbilityInputID Ability);
        
        virtual void UpdateStats() {};

        bool CanUseAbility(EAbilityInputID Ability);
        int GetAbilitySlot(EAbilityInputID Ability) const;

        bool NeedEnemyTarget(EAbilityInputID Ability);
        void SetEnemyTarget(EAbilityInputID Ability, AActor* Target);
        void ClearEnemyTarget(EAbilityInputID Ability);
        bool HasEnemyTarget(EAbilityInputID Ability);

        void UpdateCursor();
};
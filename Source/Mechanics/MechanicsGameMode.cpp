// Copyright Epic Games, Inc. All Rights Reserved.

#include "MechanicsGameMode.h"
#include "MechanicsPlayerController.h"
#include "MechanicsCharacter.h"
#include "UObject/ConstructorHelpers.h"

// AMechanicsGameMode::AMechanicsGameMode()
// {
// 	// use our custom PlayerController class
// 	PlayerControllerClass = AMechanicsPlayerController::StaticClass();

// 	// set default pawn class to our Blueprinted character
// 	static ConstructorHelpers::FClassFinder<ACharacter> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/Characters/BP_Akali"));
//     UE_LOG(LogTemp, Warning, TEXT("PlayerPawnBPClass: %s"), *PlayerPawnBPClass.Class->GetName());
// 	if (PlayerPawnBPClass.Class != nullptr)
// 	{
// 		DefaultPawnClass = PlayerPawnBPClass.Class;
// 	}

// 	// set default controller to our Blueprinted controller
// 	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/PC_Mechanics"));
//     UE_LOG(LogTemp, Warning, TEXT("PlayerControllerBPClass: %s"), *PlayerControllerBPClass.Class->GetName());
// 	if(PlayerControllerBPClass.Class != NULL)
// 	{
// 		PlayerControllerClass = PlayerControllerBPClass.Class;
// 	}
// }
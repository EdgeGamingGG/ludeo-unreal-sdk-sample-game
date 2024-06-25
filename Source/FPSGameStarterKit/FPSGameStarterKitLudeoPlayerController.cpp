#include "FPSGameStarterKitLudeoPlayerController.h"

#include "FPSGameStarterKitLudeoGameState.h"
#include "FPSGameStarterKitCheatManager.h"

AFPSGameStarterKitLudeoPlayerController::AFPSGameStarterKitLudeoPlayerController() :
	Super(),
	bIsPlayerReady(false)
{
	bAllowTickBeforeBeginPlay = true;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	CheatClass = UFPSGameStarterKitCheatManager::StaticClass();
}

void AFPSGameStarterKitLudeoPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bIsPlayerReady)
	{
		if (IsGameFullyLoaded())
		{
			bIsPlayerReady = true;

			RPCServerSignalReady();
		}
	}
}

void AFPSGameStarterKitLudeoPlayerController::OnPossess(APawn* PawnToPossess)
{
	Super::OnPossess(PawnToPossess);

	if (ACharacter* CharacterPawnToPossess = Cast<ACharacter>(PawnToPossess))
	{
		CharacterPawn = CharacterPawnToPossess;
	}
}

void AFPSGameStarterKitLudeoPlayerController::RPCServerSignalReady_Implementation()
{
	bIsPlayerReady = true;
}

bool AFPSGameStarterKitLudeoPlayerController::IsGameFullyLoaded() const
{
	if (UWorld* World = GetWorld())
	{
		if(AFPSGameStarterKitLudeoGameState* GameState = World->GetGameState<AFPSGameStarterKitLudeoGameState>())
		{
			return
			(
				PlayerState != nullptr					&&
				World->GetGameInstance() != nullptr		&&
				GameState->IsSessionReady()
			);
		}
	}

	return false;
}

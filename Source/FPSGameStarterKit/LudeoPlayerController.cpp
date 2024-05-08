#include "LudeoPlayerController.h"

#include "LudeoGameState.h"

ALudeoPlayerController::ALudeoPlayerController() :
	Super(),
	bIsPlayerReady(false)
{
	bAllowTickBeforeBeginPlay = true;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ALudeoPlayerController::Tick(float DeltaSeconds)
{
	if (!bIsPlayerReady)
	{
		if (IsGameFullyLoaded())
		{
			bIsPlayerReady = true;

			RPCServerSignalReady();
		}
	}
}

void ALudeoPlayerController::OnPossess(APawn* PawnToPossess)
{
	Super::OnPossess(PawnToPossess);

	if (ACharacter* CharacterPawnToPossess = Cast<ACharacter>(PawnToPossess))
	{
		CharacterPawn = CharacterPawnToPossess;
	}
}

void ALudeoPlayerController::RPCServerSignalReady_Implementation()
{
	bIsPlayerReady = true;
}

bool ALudeoPlayerController::IsGameFullyLoaded() const
{
	if (UWorld* World = GetWorld())
	{
		if(ALudeoGameState* GameState = World->GetGameState<ALudeoGameState>())
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

#pragma once

#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"

#include "LudeoPlayerController.generated.h"

UCLASS(config=Game)
class ALudeoPlayerController : public APlayerController
{
	GENERATED_BODY()

	UPROPERTY(Transient, SaveGame)
	ACharacter* CharacterPawn;

public:
	ALudeoPlayerController();

	virtual void Tick(float DeltaSeconds) override;

	bool IsPlayerReady() const
	{
		return bIsPlayerReady;
	}

	FORCEINLINE ACharacter* GetCharacterPawn() const
	{
		return CharacterPawn;
	}

	virtual void OnPossess(APawn* PawnToPossess) override;

private:
	UFUNCTION(Reliable, Server)
	void RPCServerSignalReady();

	bool IsGameFullyLoaded() const;

private:
	bool bIsPlayerReady;
};

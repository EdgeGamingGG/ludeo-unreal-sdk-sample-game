#pragma once

#include "GameFramework/CheatManager.h"

#include "FPSGameStarterKitCheatManager.generated.h"

UCLASS()
class UFPSGameStarterKitCheatManager : public UCheatManager
{
	GENERATED_BODY()

private:
	UFUNCTION(exec)
	void CheatSpawnActorAtRandomPosition(const FString& ClassName, const int32 NumberOfInstances);
};

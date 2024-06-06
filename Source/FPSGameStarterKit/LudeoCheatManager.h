#pragma once

#include "GameFramework/CheatManager.h"

#include "LudeoCheatManager.generated.h"

UCLASS()
class ULudeoCheatManager : public UCheatManager
{
	GENERATED_BODY()

private:
	UFUNCTION(exec)
	void CheatSpawnActorAtRandomPosition(const FString& ClassName, const int32 NumberOfInstances);
};

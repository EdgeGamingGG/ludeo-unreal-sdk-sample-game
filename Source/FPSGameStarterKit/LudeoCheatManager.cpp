#include "LudeoCheatManager.h"

#include "Kismet/KismetMathLibrary.h"

void ULudeoCheatManager::CheatSpawnActorAtRandomPosition(const FString& ClassName, const int32 NumberOfInstances)
{
	UWorld* World = GetWorld();
	check(World != nullptr)

	if (UClass* ActorClass = FindObject<UClass>(ANY_PACKAGE, *ClassName))
	{
		FActorSpawnParameters ActorSpawnParameters;
		ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		for (int32 i = 0; i < NumberOfInstances; ++i)
		{
			World->SpawnActor<AActor>
			(
				ActorClass,
				UKismetMathLibrary::RandomUnitVector() * UKismetMathLibrary::RandomFloatInRange(0.0f, 5000.0f),
				UKismetMathLibrary::RandomRotator(true),
				ActorSpawnParameters
			);
		}
	}
}

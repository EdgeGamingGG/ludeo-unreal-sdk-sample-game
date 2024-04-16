#include "Blueprint/Ludeo/LudeoBlueprintFunctionLibrary.h"

#include "Ludeo/Ludeo.h"

FString ULudeoBlueprintFunctionLibrary::LudeoGetLudeoID(const FLudeoHandle& LudeoHandle)
{
	if (const FLudeo* Ludeo = FLudeo::GetLudeoByLudeoHandle(LudeoHandle))
	{
		return Ludeo->GetLudeoID();
	}

	return FString();
}

FString ULudeoBlueprintFunctionLibrary::LudeoGetCreatorPlayerID(const FLudeoHandle& LudeoHandle)
{
	if (const FLudeo* Ludeo = FLudeo::GetLudeoByLudeoHandle(LudeoHandle))
	{
		return Ludeo->GetCreatorPlayerID();
	}

	return FString();
}

bool ULudeoBlueprintFunctionLibrary::IsValidLudeoHandle(const FLudeoHandle& LudeoHandle)
{
	return (FLudeo::GetLudeoByLudeoHandle(LudeoHandle) != nullptr);
}

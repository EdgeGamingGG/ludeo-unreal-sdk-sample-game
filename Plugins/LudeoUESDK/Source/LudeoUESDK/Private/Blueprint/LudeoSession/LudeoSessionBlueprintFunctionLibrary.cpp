#include "Blueprint/LudeoSession/LudeoSessionBlueprintFunctionLibrary.h"

#include "LudeoSession/LudeoSessionManager.h"

FLudeoSessionHandle ULudeoSessionBlueprintFunctionLibrary::CreateLudeoSession(const FCreateLudeoSessionParameters& CreateLudeoSessionParameters)
{
	if (FLudeoSessionManager* SessionManager = FLudeoSessionManager::GetInstance())
	{
		if (FLudeoSession* Session = SessionManager->CreateSession(CreateLudeoSessionParameters))
		{
			return *Session;
		}
	}

	return nullptr;
}

bool ULudeoSessionBlueprintFunctionLibrary::IsValidLudeoSessionHandle(const FLudeoSessionHandle& SessionHandle)
{
	return (FLudeoSession::GetSessionBySessionHandle(SessionHandle) != nullptr);
}

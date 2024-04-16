#include "Blueprint/LudeoSession/LudeoSessionBlueprintFunctionLibrary.h"

#include "LudeoSession/LudeoSessionManager.h"

FLudeoSessionHandle ULudeoSessionBlueprintFunctionLibrary::CreateLudeoSession()
{
	if (FLudeoSessionManager* SessionManager = FLudeoSessionManager::GetInstance())
	{
		if (FLudeoSession* Session = SessionManager->CreateSession())
		{
			return *Session;
		}
	}

	return nullptr;
}

bool ULudeoSessionBlueprintFunctionLibrary::LudeoSessionOpenGallery(const FLudeoSessionHandle& SessionHandle)
{
	if (FLudeoSession* Session = FLudeoSession::GetSessionBySessionHandle(SessionHandle))
	{
		const FLudeoResult Result = Session->OpenGallery();

		return Result.IsSuccessful();
	}

	return false;
}

bool ULudeoSessionBlueprintFunctionLibrary::IsValidLudeoSessionHandle(const FLudeoSessionHandle& SessionHandle)
{
	return (FLudeoSession::GetSessionBySessionHandle(SessionHandle) != nullptr);
}

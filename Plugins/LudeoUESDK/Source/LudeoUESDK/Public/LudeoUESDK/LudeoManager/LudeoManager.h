#pragma once

#include "Logging/LogVerbosity.h"

#include "LudeoUESDK/LudeoManager/LudeoManagerTypes.h"
#include "LudeoUESDK/LudeoSession/LudeoSessionManager.h"
#include "LudeoUESDK/LudeoLog/LudeoLog.h"

class LUDEOUESDK_API FLudeoManager
{
public:
	FLudeoManager();
	~FLudeoManager();

	static TWeakPtr<FLudeoManager> GetInstance();

	FORCEINLINE FLudeoSessionManager& GetSessionManager()
	{
		return SessionManager;
	}

	void Tick();

	FLudeoResult Initialize(const FLudeoInitializationParameters& InitializationParameters);
	FLudeoResult Finalize();

	FLudeoResult SetLoggingCallback(const LogCallbackType InLogCallback);
	FLudeoResult SetLogLevel(const FLudeoLogCategory& LogCategory, const ELogVerbosity::Type eLogLevel) const;
	FLudeoResult SetLoggingToFile(const FLudeoSetLoggingToFileParameters& SetLoggingToFileParamters) const;

private:
	FLudeoSessionManager SessionManager;

	LogCallbackType LogCallaback;
};

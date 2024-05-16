#include "LudeoUESDK/LudeoManager/LudeoManager.h"

#include "LudeoUESDK/LudeoUESDKModule.h"
#include "LudeoUESDK/LudeoCallback/LudeoCallbackManager.h"

ELogVerbosity::Type ConvertLudeoLogLevelToUnrealLogLevel(const LudeoLogLevel eLogLevel)
{
	switch (eLogLevel)
	{
		case LudeoLogLevel::Fatal:
		{
			// ELogVerbosity::Fatal will quit the game
			return ELogVerbosity::Error;
		}

		case LudeoLogLevel::Error:
		{
			return ELogVerbosity::Error;
		}

		case LudeoLogLevel::Warning:
		{
			return ELogVerbosity::Warning;
		}

		case LudeoLogLevel::Log:
		{
			return ELogVerbosity::Log;
		}

		case LudeoLogLevel::Verbose:
		{
			return ELogVerbosity::Verbose;
		}

		case LudeoLogLevel::VeryVerbose:
		{
			return ELogVerbosity::VeryVerbose;
		}

		case LudeoLogLevel::Off:
		{
			return ELogVerbosity::NoLogging;
		}
	}

	return ELogVerbosity::NoLogging;
}

LudeoLogLevel ConvertUnrealLogLevelToLudeoLogLevel(const ELogVerbosity::Type eLogLevel)
{
	switch (eLogLevel)
	{
		case ELogVerbosity::Fatal:
		{
			return LudeoLogLevel::Fatal;
		}

		case ELogVerbosity::Error:
		{
			return LudeoLogLevel::Error;
		}

		case ELogVerbosity::Warning:
		{
			return LudeoLogLevel::Warning;
		}

		case ELogVerbosity::Log:
		{
			return LudeoLogLevel::Log;
		}

		case ELogVerbosity::Verbose:
		{
			return LudeoLogLevel::Verbose;
		}

		case ELogVerbosity::VeryVerbose:
		{
			return LudeoLogLevel::VeryVerbose;
		}

		case ELogVerbosity::NoLogging:
		{
			return LudeoLogLevel::Off;
		}
	}

	return LudeoLogLevel::Off;
}

FLudeoManager::FLudeoManager() :
	LogCallaback(nullptr)
{

}

FLudeoManager::~FLudeoManager()
{
	Finalize();
}

TWeakPtr<FLudeoManager> FLudeoManager::GetInstance()
{
	if (IModuleInterface* ModuleInterface = FModuleManager::Get().GetModule(TEXT("LudeoUESDK")))
	{
		FLudeoUESDKModule& LudeoUESDKModule = *static_cast<FLudeoUESDKModule*>(ModuleInterface);

		return LudeoUESDKModule.GetLudeoManager();
	}

	return nullptr;
}

bool FLudeoManager::ExecuteLudeoCommand(const TCHAR* CommandName, const TCHAR* CommandValue)
{
	const FLudeoResult Result = ludeo_Command(TCHAR_TO_ANSI(CommandName), TCHAR_TO_ANSI(CommandValue));

	return Result.IsSuccessful();
}

bool FLudeoManager::SetEnableOverlay(const bool bIsEnabled)
{
	return ExecuteLudeoCommand(TEXT("overlay-enabled"), (bIsEnabled ? TEXT("1") : TEXT("0")));
}

bool FLudeoManager::SetEnableVideo(const bool bIsEnabled)
{
	return ExecuteLudeoCommand(TEXT("video-enabled"), (bIsEnabled ? TEXT("1") : TEXT("0")));
}

bool FLudeoManager::SetEnableMonitoring(const bool bIsEnabled)
{
	return ExecuteLudeoCommand(TEXT("monitor-enabled"), (bIsEnabled ? TEXT("1") : TEXT("0")));
}

void FLudeoManager::Tick()
{
	ludeo_Tick();
}

FLudeoResult FLudeoManager::Initialize(const FLudeoInitializationParameters& InitializationParameters)
{
	LudeoInitializeParams InternalLudeoInitializeParams = Ludeo::create<LudeoInitializeParams>();

	if (
		InitializationParameters.AllocateMemoryFunction != nullptr		||
		InitializationParameters.ReAllocateMemoryFunction != nullptr	||
		InitializationParameters.FreeMemoryFunction != nullptr
	)
	{
		InternalLudeoInitializeParams.memoryAllocFunc = InitializationParameters.AllocateMemoryFunction;
		InternalLudeoInitializeParams.memoryReallocFunc = InitializationParameters.ReAllocateMemoryFunction;
		InternalLudeoInitializeParams.memoryFreeFunc = InitializationParameters.FreeMemoryFunction;
	}
	else
	{
		InternalLudeoInitializeParams.memoryAllocFunc = [](size_t Size)
		{
			return FMemory::Malloc(static_cast<SIZE_T>(Size));
		};

		InternalLudeoInitializeParams.memoryReallocFunc = [](void* AllocatedMemory, size_t Size)
		{
			return FMemory::Realloc(AllocatedMemory, static_cast<SIZE_T>(Size));
		};

		InternalLudeoInitializeParams.memoryFreeFunc = [](void* AllocatedMemory)
		{
			return FMemory::Free(AllocatedMemory);
		};
	}

	const FLudeoResult Result = ludeo_Initialize(&InternalLudeoInitializeParams);

	if (Result.IsSuccessful())
	{
		SetLoggingCallback([](const FLudeoLogMessage& LogMessage)
		{
			FMsg::Logf_Internal
			(
				nullptr,
				0,
				*FString::Printf(TEXT("[Ludeo] %s"), UTF8_TO_TCHAR(LogMessage.LogCategory.ToString().GetData())),
				LogMessage.LogLevel,
				TEXT("%s"),
				UTF8_TO_TCHAR(LogMessage.Message.GetData())
			);
		});
	}

	return Result;
}

FLudeoResult FLudeoManager::Finalize()
{
	SessionManager.Finalize();

	FLudeoCallbackManager::GetInstance().Finalize();

	LogCallaback = nullptr;

	return ludeo_Shutdown();
}

FLudeoResult FLudeoManager::SetLoggingCallback(const LogCallbackType InLogCallback)
{
	FLudeoResult Result(LudeoResult::Unknown);
	{
		const TGuardValue<LogCallbackType> GuardedCallbackValue(LogCallaback, InLogCallback);

		if (LogCallaback != nullptr)
		{
			Result = ludeo_SetLoggingCallback([](const LudeoLogMessage* pLogMessage)
			{
				if (const TSharedPtr<FLudeoManager> LudeoManager = FLudeoManager::GetInstance().Pin())
				{
					if (LudeoManager->LogCallaback != nullptr)
					{
						FLudeoLogMessage LogMessage;
						LogMessage.LogLevel = ConvertLudeoLogLevelToUnrealLogLevel(pLogMessage->level),
						LogMessage.LogCategory = pLogMessage->category;
						LogMessage.Message = pLogMessage->message;
						LogMessage.Timestamp = pLogMessage->timestamp;

						LudeoManager->LogCallaback(LogMessage);
					}
				}
			});
		}
		else
		{
			Result = ludeo_SetLoggingCallback(nullptr);
		}
	}

	if (Result.IsSuccessful())
	{
		LogCallaback = InLogCallback;
	}

	return Result;
}

FLudeoResult FLudeoManager::SetLogLevel(const FLudeoLogCategory& LogCategory, const ELogVerbosity::Type eLogLevel) const
{
	return ludeo_SetLoggingLevel(LogCategory, ConvertUnrealLogLevelToLudeoLogLevel(eLogLevel));
}

FLudeoResult FLudeoManager::SetLoggingToFile(const FLudeoSetLoggingToFileParameters& SetLoggingToFileParamters) const
{
	const FTCHARToUTF8 DirectoryStringConverter
	(
		SetLoggingToFileParamters.Directory.GetCharArray().GetData(),
		SetLoggingToFileParamters.Directory.GetCharArray().Num()
	);

	LudeoSetLoggingToFileParams SetLoggingToFileParams = Ludeo::create<LudeoSetLoggingToFileParams>();
	SetLoggingToFileParams.enable = static_cast<LudeoBool>(SetLoggingToFileParamters.bEnable);
	SetLoggingToFileParams.directory = DirectoryStringConverter.Get();

	return ludeo_SetLoggingToFile(&SetLoggingToFileParams);
}

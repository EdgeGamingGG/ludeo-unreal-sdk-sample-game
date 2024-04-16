#pragma once

#include "Modules/ModuleManager.h"

#include "LudeoManager/LudeoManager.h"

class FLudeoUESDKModule : public IModuleInterface
{
public:
	using Super = IModuleInterface;

	FLudeoUESDKModule()
	{
	}

	virtual ~FLudeoUESDKModule()
	{
	}

	virtual void StartupModule() override;

	virtual void ShutdownModule() override;

	TWeakPtr<FLudeoManager> GetLudeoManager() const
	{
		return LudeoManager;
	}

private:
	TSharedPtr<FLudeoManager> LudeoManager;
};

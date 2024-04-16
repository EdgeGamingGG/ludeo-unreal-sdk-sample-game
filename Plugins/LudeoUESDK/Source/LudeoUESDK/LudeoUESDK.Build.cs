using UnrealBuildTool;
using System.IO;

// If we need to use Tools.DotNETCommon, it seems that is not available in UE5, so the same Build.cs file won't work for both UE4 and UE5.
// See: https://dev.epicgames.com/community/snippets/7YD/ue4-to-ue5-plugin-upgrade-notes
// using Tools.DotNETCommon;

public class LudeoUESDK : ModuleRules
{
	public LudeoUESDK(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		bEnableExceptions = true;

		//
		// Useful documentation for this at https://docs.unrealengine.com/4.27/en-US/ProductionPipelines/BuildTools/UnrealBuildTool/ThirdPartyLibraries/
		//

		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			});
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			});

		PublicDependencyModuleNames.AddRange(
			new string[] {
				"Core", 
				"CoreUObject",
				"Engine",
				"LudeoSDK"
			});

		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"SlateCore"
			});
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[] {
				// ... add any modules that your module loads dynamically here ...
			});
	}
}


using UnrealBuildTool;
using System.IO;

// If we need to use Tools.DotNETCommon, it seems that is not available in UE5, so the same Build.cs file won't work for both UE4 and UE5.
// See: https://dev.epicgames.com/community/snippets/7YD/ue4-to-ue5-plugin-upgrade-notes
// using Tools.DotNETCommon;

public class LudeoUESDKTest : ModuleRules
{
	public LudeoUESDKTest(ReadOnlyTargetRules Target) : base(Target)
	{
		CppStandard = CppStandardVersion.Cpp17;

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
				Path.Combine(ModuleDirectory, "ThirdParty", "PolyHook_2", "Include")
				// ... add other private include paths required here ...
			});

		PublicDependencyModuleNames.AddRange(
			new string[] {
				"Core", 
				"CoreUObject",
				"Engine"
			});

		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"SlateCore",
				"LudeoUESDK",
				"Json",
				"JsonUtilities"
			});
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[] {
				// ... add any modules that your module loads dynamically here ...
			});
			
		string[] LibraryFilePathCollection = Directory.GetFiles
		(
			Path.Combine(ModuleDirectory, "ThirdParty", "PolyHook_2", "Libraries", Target.Platform.ToString()),
			"*.*",
			SearchOption.AllDirectories
		);

		PublicAdditionalLibraries.AddRange(LibraryFilePathCollection);
	}
}


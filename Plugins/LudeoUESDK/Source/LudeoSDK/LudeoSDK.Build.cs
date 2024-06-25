using System;
using System.IO;
using UnrealBuildTool;

public class LudeoSDK : ModuleRules
{
	public string SDKBaseDir
	{
		get
		{
			return Path.Combine(ModuleDirectory, "SDK");
		}
	}

	public string SDKIncludeDir
	{
		get
		{
			return Path.Combine(SDKBaseDir, "Include");
		}
	}

	public string SDKBinariesDir
	{
		get
		{
			return Path.Combine(SDKBaseDir, "Bin");
		}
	}

	public string SDKLibsDir
	{
		get
		{
			return Path.Combine(SDKBaseDir, "Lib");
		}
	}

	public string SDKThirdPartyDir
	{
		get
		{
			return Path.Combine(SDKBaseDir, "ThirdParty");
		}
	}

	public bool UseDebugSDK
	{
		get
		{
			return
			(
				Target.Configuration == UnrealTargetConfiguration.DebugGame ||
				Target.Configuration == UnrealTargetConfiguration.Debug		||
				File.Exists(Path.Combine(PluginDirectory, "debug_uses_debug.txt"))
			);
		}
	}

	public string LibraryConfiguration
	{
		get
		{
			if (UseDebugSDK)
			{
				return "Debug";
			}
			else if (Target.Configuration == UnrealTargetConfiguration.Development)
			{
				return "Development";
			}
			else
			{
				return "Release";
			}
		}
	}

	public string LibraryLinkNameBase
	{
		get
		{
			return string.Format("LudeoSDK-{0}-{1}", Target.Platform.ToString(), LibraryConfiguration);
		}
	}

	public string StaticLibraryExtension
	{
		get
		{
			if (Target.Platform.IsInGroup(UnrealPlatformGroup.Microsoft))
			{
				return ".lib";
			}

			throw new BuildException("Unsupported platform");
		}
	}

	public string DynamicLibraryExtension
	{
		get
		{
			if (Target.Platform.IsInGroup(UnrealPlatformGroup.Microsoft))
			{
				return ".dll";
			}

			throw new BuildException("Unsupported platform");
		}
	}

	public string DynamicLibraryDebugSymbolExtension
	{
		get
		{
			if (Target.Platform.IsInGroup(UnrealPlatformGroup.Microsoft))
			{
				return ".pdb";
			}

			throw new BuildException("Unsupported platform");
		}
	}

	public string LibraryLinkName
	{
		get
		{
			if (Target.Platform.IsInGroup(UnrealPlatformGroup.Microsoft))
			{
				return Path.Combine(SDKLibsDir, LibraryLinkNameBase + StaticLibraryExtension);
			}

			throw new BuildException("Unsupported platform");
		}
	}

	public string RuntimeLibraryFileName
	{
		get
		{
			if (Target.Platform.IsInGroup(UnrealPlatformGroup.Microsoft))
			{
				return LibraryLinkNameBase + DynamicLibraryExtension;
			}

			throw new BuildException("Unsupported platform");
		}
	}

	public string RuntimeLibraryDebugSymbolFileName
	{
		get
		{
			if (Target.Platform.IsInGroup(UnrealPlatformGroup.Microsoft))
			{
				return LibraryLinkNameBase + DynamicLibraryDebugSymbolExtension;
			}

			throw new BuildException("Unsupported platform");
		}
	}

	public LudeoSDK(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;

		PublicIncludePaths.Add(SDKIncludeDir);

		PublicAdditionalLibraries.Add(LibraryLinkName);
		RuntimeDependencies.Add
		(
			Path.Combine("$(BinaryOutputDir)", RuntimeLibraryFileName),
			Path.Combine(SDKBinariesDir, RuntimeLibraryFileName)
		);
		RuntimeDependencies.Add
		(
			Path.Combine("$(BinaryOutputDir)", RuntimeLibraryDebugSymbolFileName),
			Path.Combine(SDKBinariesDir, RuntimeLibraryDebugSymbolFileName)
		);

		string[] BinaryFilePathCollection = Directory.GetFiles
		(
			Path.Combine(SDKBaseDir, "ThirdParty", Target.Platform.ToString(), LibraryConfiguration),
			"*.*",
			SearchOption.AllDirectories
		);

		foreach (string BinaryFilePath in BinaryFilePathCollection)
		{
			string Extension = Path.GetExtension(BinaryFilePath).ToLower();
			
			if (Extension == StaticLibraryExtension)
			{
				PublicAdditionalLibraries.Add(BinaryFilePath);
			}
			else
			{
				RuntimeDependencies.Add
				(
					Path.Combine("$(BinaryOutputDir)", Path.GetFileName(BinaryFilePath)),
					BinaryFilePath
				);
			}
		}

		// Setting this to 1 enables various to_string(xxx) functions, so the SDK user doesn't need to call e.g: ludeo_ResultToString
		PublicDefinitions.Add("LUDEOSDK_TO_STRING=1");
		// Setting this
		PublicDefinitions.Add("LUDEOSDK_TO_STRING_TYPE=FString");

		PublicDefinitions.Add(string.Format("LUDEOSDK_DLL_NAME=\"{0}\"", RuntimeLibraryFileName));
	}

};


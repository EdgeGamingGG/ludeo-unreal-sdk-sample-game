using System;
using System.IO;
using UnrealBuildTool;

public class LudeoSDK : ModuleRules
{
	public virtual string SDKBaseDir
	{
		get
		{
			return Path.Combine(ModuleDirectory, "SDK");
		}
	}

	public virtual string SDKIncludeDir
	{
		get
		{
			return Path.Combine(SDKBaseDir, "Include");
		}
	}

	public virtual string SDKBinariesDir
	{
		get
		{
			return Path.Combine(SDKBaseDir, "Bin");
		}
	}

	public virtual string SDKLibsDir
	{
		get
		{
			return Path.Combine(SDKBaseDir, "Lib");
		}
	}

	public virtual string SDKThirdPartyDir
	{
		get
		{
			return Path.Combine(SDKBaseDir, "ThirdParty");
		}
	}

	public virtual bool UseDebugSDK
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

	public string LibraryLinkNameBase
	{
		get
		{
			if (UseDebugSDK)
			{
				return string.Format("LudeoSDK-{0}-Debug", Target.Platform.ToString());
			}
			else if(Target.Configuration == UnrealTargetConfiguration.Development)
			{
				return string.Format("LudeoSDK-{0}-Development", Target.Platform.ToString());
			}
			else
			{
				return string.Format("LudeoSDK-{0}-Release", Target.Platform.ToString());
			}
		}
	}

	public virtual string StaticLibraryExtension
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

	public virtual string DynamicLibraryExtension
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

	public virtual string DynamicLibraryDebugSymbolExtension
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

	public virtual string LibraryLinkName
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

	public virtual string RuntimeLibraryFileName
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

	public virtual string RuntimeLibraryDebugSymbolFileName
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

	public virtual bool bRequiresRuntimeLoad
	{
		get
		{
			return Target.Platform.IsInGroup(UnrealPlatformGroup.Windows) || Target.Platform == UnrealTargetPlatform.Mac;
			// Other platforms may override this property.
		}
	}

	public LudeoSDK(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;

		PublicIncludePaths.Add(SDKIncludeDir);

		PublicAdditionalLibraries.Add(LibraryLinkName);
		RuntimeDependencies.Add
		(
			Path.Combine("$(TargetOutputDir)", RuntimeLibraryFileName),
			Path.Combine(SDKBinariesDir, RuntimeLibraryFileName)
		);
		RuntimeDependencies.Add
		(
			Path.Combine("$(TargetOutputDir)", RuntimeLibraryDebugSymbolFileName),
			Path.Combine(SDKBinariesDir, RuntimeLibraryDebugSymbolFileName)
		);

		if (bRequiresRuntimeLoad)
		{
			PublicDelayLoadDLLs.Add(RuntimeLibraryFileName);
			PublicDelayLoadDLLs.Add(RuntimeLibraryDebugSymbolFileName);
		}

		string[] BinaryFilePathCollection = Directory.GetFiles
		(
			Path.Combine(SDKBaseDir, "ThirdParty", Target.Platform.ToString()),
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
					Path.Combine("$(TargetOutputDir)", Path.GetFileName(BinaryFilePath)),
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


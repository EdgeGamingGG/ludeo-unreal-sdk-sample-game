// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class FPSGameStarterKitTarget : TargetRules
{
	public FPSGameStarterKitTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		bUseLoggingInShipping = true;

		ExtraModuleNames.AddRange( new string[] { "FPSGameStarterKit" } );
	}
}

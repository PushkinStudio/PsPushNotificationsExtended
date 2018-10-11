// Copyright 2018 Mail.Ru Group. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class PsPushNotificationsExtended : ModuleRules
{
	public PsPushNotificationsExtended(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateIncludePaths.Add("PsPushNotificationsExtended/Private");

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
			});

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
			});

		if (Target.Platform == UnrealTargetPlatform.Mac)
		{
			PrivateIncludePaths.Add("PsPushNotificationsExtended/Private/Mac");
		}
		else if (Target.Platform == UnrealTargetPlatform.Win32 ||
				Target.Platform == UnrealTargetPlatform.Win64)
		{
			PrivateIncludePaths.Add("PsPushNotificationsExtended/Private/Windows");
		}
		else if (Target.Platform == UnrealTargetPlatform.IOS)
		{
			PublicFrameworks.AddRange(
				new string[]
				{
					"SystemConfiguration",
					"UIKit",
					"Foundation",
					"MobileCoreServices",
					"CFNetwork",
					"CoreData",
					"CoreFoundation"
				}
			);

			PrivateIncludePaths.Add("PsPushNotificationsExtended/Private/IOS");
		}
		else if (Target.Platform == UnrealTargetPlatform.Android)
		{
			string PluginPath = Utils.MakePathRelativeTo(ModuleDirectory, Target.RelativeEnginePath);
			AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(PluginPath, "PsPushNotifications_APL.xml"));
			PrivateIncludePaths.Add("PsPushNotificationsExtended/Private/Android");
		}
	}
}

// Copyright 2018 Mail.Ru Group. All Rights Reserved.

using UnrealBuildTool;

public class PsPushNotificationsExtended : ModuleRules
{
	public PsPushNotificationsExtended(ReadOnlyTargetRules Target) : base(Target)
	{
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

		if (Target.Platform == UnrealTargetPlatform.IOS)
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
		}
	}
}

// Copyright 2018 Mail.Ru Group. All Rights Reserved.

#include "PsPushNotificationsExtendedPrivatePCH.h"
#include "PsPushNotificationsExtended.h"
#include "PsPushNotificationsExtendedManager.h"
#include "UObject/UObjectGlobals.h"
#include "PsPushNotificationsExtendedSettings.h"
#include "Developer/Settings/Public/ISettingsModule.h"
#include "EngineMinimal.h"

#define LOCTEXT_NAMESPACE "PsPushNotifications"

void FPsPushNotificationsExtended::StartupModule()
{
	Settings = NewObject<UPsPushNotificationsExtendedSettings>(GetTransientPackage(), "PushNotificationsExtendedSettings", RF_Standalone);
	Settings->AddToRoot();

	// No UOBJECT system at the moment of StartupModule call, need to read settings from config manually
	Settings->ReadFromConfig();

	// Register settings for editor
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "PushNotificationsExtended",
			LOCTEXT("RuntimeSettingsName", "Push notifications extended Service"),
			LOCTEXT("RuntimeSettingsDescription", "Configure for extended push notifications"),
			GetMutableDefault<UPsPushNotificationsExtendedSettings>());
	}
}

void FPsPushNotificationsExtended::ShutdownModule()
{
	// Shuttin down Push manager
	if (!GExitPurge && UPsPushNotificationsExtendedManager::GetInstance())
	{
		// if GExitPurge - manager is already removed
		UPsPushNotificationsExtendedManager::GetInstance()->RemoveFromRoot();
		Settings->RemoveFromRoot();
	}
}

IMPLEMENT_MODULE(FPsPushNotificationsExtended, PsPushNotificationsExtended)

#undef LOCTEXT_NAMESPACE

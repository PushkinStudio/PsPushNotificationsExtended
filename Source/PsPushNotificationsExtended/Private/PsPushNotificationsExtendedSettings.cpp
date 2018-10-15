// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.

#include "PsPushNotificationsExtendedPrivatePCH.h"
#include "PsPushNotificationsExtendedSettings.h"

UPsPushNotificationsExtendedSettings::UPsPushNotificationsExtendedSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UPsPushNotificationsExtendedSettings::ReadFromConfig()
{
	const FString ConfigSection = FString("/Script/PsPushNotificationsExtended.PsPushNotificationsExtendedSettings");

	// Android specific settings
	GConfig->GetString(*ConfigSection, TEXT("PsPushNotificationsAndroidChannelID"), PsPushNotificationsAndroidChannelID, GEngineIni);
	GConfig->GetString(*ConfigSection, TEXT("PsPushNotificationsAndroidChannelName"), PsPushNotificationsAndroidChannelName, GEngineIni);
	GConfig->GetString(*ConfigSection, TEXT("PsPushNotificationsAndroidChannelDescription"), PsPushNotificationsAndroidChannelDescription, GEngineIni);
}

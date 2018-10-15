// Copyright 2018 Mail.Ru Group. All Rights Reserved.
#pragma once

#include "PsPushNotificationsExtendedSettings.generated.h"

UCLASS(config = Engine, defaultconfig)
class UPsPushNotificationsExtendedSettings : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	/** Read settings from config */
	void ReadFromConfig();

	/** Push notifications android channel ID */
	UPROPERTY(Config, EditAnywhere, Category = "Android", meta = (DisplayName = "Push notifications channel ID (optional)"))
	FString PsPushNotificationsAndroidChannelID;

	/** Push notifications android channel name */
	UPROPERTY(Config, EditAnywhere, Category = "Android", meta = (DisplayName = "Push notifications channel name"))
	FString PsPushNotificationsAndroidChannelName;

	/** Push notifications android channel description */
	UPROPERTY(Config, EditAnywhere, Category = "Android", meta = (DisplayName = "Android channel description (optional)"))
	FString PsPushNotificationsAndroidChannelDescription;
};

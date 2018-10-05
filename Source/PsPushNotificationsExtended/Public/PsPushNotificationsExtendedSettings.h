// Copyright 2018 Mail.Ru Group. All Rights Reserved.
#pragma once

#include "PsPushNotificationsExtendedSettings.generated.h"

UCLASS(config = Engine, defaultconfig)
class UPsPushNotificationsExtendedSettings : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	/** Push notifications android channel ID */
	UPROPERTY(Config, EditAnywhere)
	FString PsPushNotificationsAndroidChannelID;

	/** Push notifications android channel name */
	UPROPERTY(Config, EditAnywhere)
	FString PsPushNotificationsAndroidChannelName;

	/** Push notifications android channel description */
	UPROPERTY(Config, EditAnywhere)
	FString PsPushNotificationsAndroidChannelDescription;
};

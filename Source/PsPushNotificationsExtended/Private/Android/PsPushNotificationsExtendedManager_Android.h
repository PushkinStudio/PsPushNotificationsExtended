// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.
#pragma once

#include "PsPushNotificationsExtendedManager.h"
#include "CoreTypes.h"
#include "Delegates/Delegate.h"
#include "PsPushNotificationsExtendedTypes.h"
#include "PsPushNotificationsExtendedManager_Android.generated.h"

UCLASS()
class UPsPushNotificationsExtendedManagerAndroid : public UPsPushNotificationsExtendedManager
{
	GENERATED_UCLASS_BODY()

public:
	/** Requests authority for push notifications */
	void RequestPushNotifications() override;

	/** Adds notifications cathegory */
	void AddNotificationCategory(const FString& Name, const TArray<FPsNotificationsAction>& Actions) override;

	/** Sends local notifications with a specified content in a period of time in seconds
	 @return Notification ID
	 */
	FString SendLocalNotificationFromNow(float SecondsFromNow, const FPsNotification& Notification) override;

	/** Sends local notifications with a specified content at specified time
	 @return Notification ID
	 */
	FString SendLocalNotification(const FDateTime& DateTime, bool bLocalTime, const FPsNotification& Notification) override;

	/** Cleares all notifications */
	void ClearAllLocalNotifications() override;

	/** Removes local notification with specified ID */
	void ClearLocalNotificationsWithId(const TArray<FString>& NotificationsIds) override;

	/** Gets last notification action id (use at start of the application to navigate user action) */
	FString GetLastNotificationActivationCode() override;

	/** Gets last notification action id (use at start of the application to navigate user action) */
	FString GetLastNotificationActionId() override;
};

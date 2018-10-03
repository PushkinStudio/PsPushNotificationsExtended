// Copyright 2018 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CoreTypes.h"
#include "PsPushNotificationsExtendedTypes.h"

#if USE_ANDROID_JNI
#include <jni.h>
#include <android/log.h>

class FPsPushNotificationsExtendedJavaWrapper
{
public:
	static jmethodID PsPushNotificationsExtended_Init;
	static jmethodID PsPushNotificationsExtended_LocalNotificationScheduleAtTime;
	static jmethodID PsPushNotificationsExtended_LocalNotificationAddActionToCategory;
	static jmethodID PsPushNotificationsExtended_LocalNotificationClearAll;
	static jmethodID PsPushNotificationsExtended_LocalNotificationClearById;
	static jmethodID PsPushNotificationsExtended_LastNotificationActionId;

	/** Initialize notifications channel */
	static void Init();
	/** Schedule local notification */
	static FString LocalNotificationScheduleAtTime(const FDateTime& DateTime, bool bIsLocalTime, const FString& Title, const FString& Body, const FString& ActivationEvent, const FString& Category, const FString& ContentURL);
	/** Add category for notification */
	static void LocalNotificationAddCategory(const FString& CategoryId, const TArray<FPsNotificationsAction>& Actions);
	/** Clear all notifications on android */
	static void ClearAllNotifications();
	/** Get last notification action */
	static FString GetLastNotificationAction();
};


#endif // USE_ANDROID_JNI

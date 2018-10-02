// Copyright 2018 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#if USE_ANDROID_JNI
#include <jni.h>
#include <android/log.h>

class FPsPushNotificationsExtendedJavaWrapper
{
	static void RegisterClassesAndMethods();

	static jmethodID AndroidPsPushNotificationsExtended_LocalNotificationScheduleAtTime;
	static jmethodID AndroidPsPushNotificationsExtended_LocalNotificationClearAll;
	static jmethodID AndroidPsPushNotificationsExtended_LocalNotificationClearById;
};

#endif // USE_ANDROID_JNI

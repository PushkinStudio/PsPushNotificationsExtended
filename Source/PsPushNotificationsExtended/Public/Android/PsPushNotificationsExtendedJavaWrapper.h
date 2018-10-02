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
	static jmethodID PsPushNotificationsExtended_LocalNotificationClearAll;
	static jmethodID PsPushNotificationsExtended_LocalNotificationClearById;

	static void Init();
	static FString LocalNotificationScheduleAtTime(const FDateTime& DateTime, bool bIsLocalTime, const FString& Title, const FString& Body, const FString& ActivationEvent);

private:

	static TMap<FString, TArray<FPsNotificationsAction>> RegisteredActions;

};


#endif // USE_ANDROID_JNI

// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.

#include "Android/AndroidJNI.h" // Engine class

#if USE_ANDROID_JNI
#include "Android/AndroidPlatformCrashContext.h"
#include "Android/AndroidApplication.h"
#include "Android/AndroidInputInterface.h"
#include "UnrealEngine.h"

void FPsPushNotificationsExtendedJavaWrapper::FindClassesAndMethods()
{
	AndroidPsPushNotificationsExtended_LocalNotificationScheduleAtTime = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_LocalNotificationScheduleAtTime", "(Ljava/lang/String;ZLjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V", bIsOptional);
	AndroidPsPushNotificationsExtended_LocalNotificationClearAll = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_LocalNotificationClearAll", "()V", bIsOptional);;
	AndroidPsPushNotificationsExtended_LocalNotificationClearById = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_LocalNotificationDestroyIfExists", "(I)Z", bIsOptional);;
}

#endif // USE_ANDROID_JNI

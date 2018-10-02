// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.

#include "../PsPushNotificationsExtendedPrivatePCH.h"
#include "Android/PsPushNotificationsExtendedJavaWrapper.h"
#include "PsPushNotificationsExtendedManager.h"

#if USE_ANDROID_JNI

#include "Runtime/Launch/Public/Android/AndroidJNI.h"
#include "AndroidApplication.h"
#include "UnrealEngine.h"
#include <jni.h>

void FPsPushNotificationsExtendedJavaWrapper::Init()
{
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv(true))
	{
		UE_LOG(LogPsPushNotificationsExtended, Log, TEXT("FPsPushNotificationsExtendedJavaWrapper::Init java call"));

		// Find init method
		FPsPushNotificationsExtendedJavaWrapper::PsPushNotificationsExtended_Init = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "PsPushNotificationsExtended_Init", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V", false);

		// Find send local notifications method
		FPsPushNotificationsExtendedJavaWrapper::PsPushNotificationsExtended_LocalNotificationScheduleAtTime = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "PsPushNotificationsExtended_LocalNotificationScheduleAtTime", "(Ljava/lang/String;ZLjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;", false);

		const FString ChannelId = TEXT("");
		const FString ChannelName = TEXT("Channel Name");
		const FString ChannelDescription = TEXT("Channel Description");

		jstring JChannelId = Env->NewStringUTF(TCHAR_TO_UTF8(*ChannelId));
		jstring JChannelName = Env->NewStringUTF(TCHAR_TO_UTF8(*ChannelName));
		jstring JChannelDescription = Env->NewStringUTF(TCHAR_TO_UTF8(*ChannelDescription));

		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, FPsPushNotificationsExtendedJavaWrapper::PsPushNotificationsExtended_Init, JChannelId, JChannelName, JChannelDescription);

		Env->DeleteLocalRef(JChannelId);
		Env->DeleteLocalRef(JChannelName);
		Env->DeleteLocalRef(JChannelDescription);
	}
}

FString FPsPushNotificationsExtendedJavaWrapper::LocalNotificationScheduleAtTime(const FDateTime& DateTime, bool bIsLocalTime, const FString& Title, const FString& Body, const FString& ActivationEvent)
{
	FString NotificationId;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv(true))
	{
		const FString FireDateTimeFormatted = FString::FromInt(DateTime.GetYear()) + "-" + FString::FromInt(DateTime.GetMonth()) + "-" + FString::FromInt(DateTime.GetDay()) + " " + FString::FromInt(DateTime.GetHour()) + ":" + FString::FromInt(DateTime.GetMinute()) + ":" + FString::FromInt(DateTime.GetSecond());
		const FString ActionStr = TEXT("ActionField");

		jstring JFireDateTime = Env->NewStringUTF(TCHAR_TO_UTF8(*FireDateTimeFormatted));
		jstring JTitle = Env->NewStringUTF(TCHAR_TO_UTF8(*Title));
		jstring JBody = Env->NewStringUTF(TCHAR_TO_UTF8(*Body));
		jstring JAction = Env->NewStringUTF(TCHAR_TO_UTF8(*ActionStr));
		jstring JActivationEvent = Env->NewStringUTF(TCHAR_TO_UTF8(*ActivationEvent));

		jstring JNotificationId = (jstring)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, FPsPushNotificationsExtendedJavaWrapper::PsPushNotificationsExtended_LocalNotificationScheduleAtTime, JFireDateTime, bIsLocalTime, JTitle, JBody, JAction, JActivationEvent);

		if (JNotificationId)
		{
			const char *nativeString = Env->GetStringUTFChars(JNotificationId, 0);
			if (nativeString)
			{
				NotificationId = FString(nativeString);
			}
		}
		UE_LOG(LogPsPushNotificationsExtended, Log, TEXT("FPsPushNotificationsExtendedJavaWrapper::SendLocalNotification java call ID %s"), *NotificationId);

		Env->DeleteLocalRef(JFireDateTime);
		Env->DeleteLocalRef(JTitle);
		Env->DeleteLocalRef(JBody);
		Env->DeleteLocalRef(JActivationEvent);
	}

	return NotificationId;
}

jmethodID FPsPushNotificationsExtendedJavaWrapper::PsPushNotificationsExtended_Init;
jmethodID FPsPushNotificationsExtendedJavaWrapper::PsPushNotificationsExtended_LocalNotificationScheduleAtTime;
jmethodID FPsPushNotificationsExtendedJavaWrapper::PsPushNotificationsExtended_LocalNotificationClearAll;
jmethodID FPsPushNotificationsExtendedJavaWrapper::PsPushNotificationsExtended_LocalNotificationClearById;

#endif // USE_ANDROID_JNI

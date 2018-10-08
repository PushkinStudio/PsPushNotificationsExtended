// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.

#include "../PsPushNotificationsExtendedPrivatePCH.h"
#include "Android/PsPushNotificationsExtendedJavaWrapper.h"
#include "PsPushNotificationsExtendedManager.h"
#include "PsPushNotificationsExtendedSettings.h"

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
		FPsPushNotificationsExtendedJavaWrapper::PsPushNotificationsExtended_LocalNotificationScheduleAtTime = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "PsPushNotificationsExtended_LocalNotificationScheduleAtTime", "(Ljava/lang/String;ZLjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;", false);

		// Find add category method
		FPsPushNotificationsExtendedJavaWrapper::PsPushNotificationsExtended_LocalNotificationAddActionToCategory = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "PsPushNotificationsExtended_LocalNotificationAddCategory", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V", false);

		// Clear all method
		FPsPushNotificationsExtendedJavaWrapper::PsPushNotificationsExtended_LocalNotificationClearAll = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "PsPushNotificationsExtended_LocalNotificationClearAll", "()V", false);

		// Get action method
		FPsPushNotificationsExtendedJavaWrapper::PsPushNotificationsExtended_LastNotificationActivationCode = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "PsPushNotificationsExtended_LastNotificationActivationCode", "()Ljava/lang/String;", false);

		// Get action method
		FPsPushNotificationsExtendedJavaWrapper::PsPushNotificationsExtended_LastNotificationActionId = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "PsPushNotificationsExtended_LastNotificationActionId", "()Ljava/lang/String;", false);

		const FString ChannelId = GetDefault<UPsPushNotificationsExtendedSettings>()->PsPushNotificationsAndroidChannelID;
		const FString ChannelName = GetDefault<UPsPushNotificationsExtendedSettings>()->PsPushNotificationsAndroidChannelName;
		const FString ChannelDescription = GetDefault<UPsPushNotificationsExtendedSettings>()->PsPushNotificationsAndroidChannelDescription;

		jstring JChannelId = Env->NewStringUTF(TCHAR_TO_UTF8(*ChannelId));
		jstring JChannelName = Env->NewStringUTF(TCHAR_TO_UTF8(*ChannelName));
		jstring JChannelDescription = Env->NewStringUTF(TCHAR_TO_UTF8(*ChannelDescription));

		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, FPsPushNotificationsExtendedJavaWrapper::PsPushNotificationsExtended_Init, JChannelId, JChannelName, JChannelDescription);

		Env->DeleteLocalRef(JChannelId);
		Env->DeleteLocalRef(JChannelName);
		Env->DeleteLocalRef(JChannelDescription);
	}
}

FString FPsPushNotificationsExtendedJavaWrapper::LocalNotificationScheduleAtTime(const FDateTime& DateTime, bool bIsLocalTime, const FString& Title, const FString& Body, const FString& ActivationEvent, const FString& Category, const FString& ContentURL)
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
		jstring JCategory = Env->NewStringUTF(TCHAR_TO_UTF8(*Category));
		jstring JContentURL = Env->NewStringUTF(TCHAR_TO_UTF8(*ContentURL));

		jstring JNotificationId = (jstring)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, FPsPushNotificationsExtendedJavaWrapper::PsPushNotificationsExtended_LocalNotificationScheduleAtTime, JFireDateTime, bIsLocalTime, JTitle, JBody, JAction, JActivationEvent, JCategory, JContentURL);

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

void FPsPushNotificationsExtendedJavaWrapper::LocalNotificationAddCategory(const FString& CategoryId, const TArray<FPsNotificationsAction>& Actions)
{
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv(true))
	{
		jstring JCategoryId = Env->NewStringUTF(TCHAR_TO_UTF8(*CategoryId));

		// Adding actions to category
		for (const FPsNotificationsAction& Action : Actions)
		{
			jstring JActionName = Env->NewStringUTF(TCHAR_TO_UTF8(*Action.Title));
			jstring JActionId = Env->NewStringUTF(TCHAR_TO_UTF8(*Action.ActionId));

			FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, FPsPushNotificationsExtendedJavaWrapper::PsPushNotificationsExtended_LocalNotificationAddActionToCategory, JCategoryId, JActionName, JActionId);

			Env->DeleteLocalRef(JActionName);
			Env->DeleteLocalRef(JActionId);
		}

		Env->DeleteLocalRef(JCategoryId);
	}
}

void FPsPushNotificationsExtendedJavaWrapper::ClearAllNotifications()
{
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv(true))
	{
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, PsPushNotificationsExtended_LocalNotificationClearAll);
	}
}

FString FPsPushNotificationsExtendedJavaWrapper::GetLastNotificationActivationCode()
{
	FString ActivationCode;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv(true))
	{
		jstring JActivationCode = (jstring)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, FPsPushNotificationsExtendedJavaWrapper::PsPushNotificationsExtended_LastNotificationActivationCode);
		
		if (JActivationCode)
		{
			const char *nativeString = Env->GetStringUTFChars(JActivationCode, 0);
			if (nativeString)
			{
				ActivationCode = FString(nativeString);
			}
		}
	}
	return ActivationCode;
}

FString FPsPushNotificationsExtendedJavaWrapper::GetLastNotificationAction()
{
	FString ActionId;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv(true))
	{
		jstring JActionId = (jstring)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, FPsPushNotificationsExtendedJavaWrapper::PsPushNotificationsExtended_LastNotificationActionId);

		if (JActionId)
		{
			const char *nativeString = Env->GetStringUTFChars(JActionId, 0);
			if (nativeString)
			{
				ActionId = FString(nativeString);
			}
		}
	}
	return ActionId;
}

jmethodID FPsPushNotificationsExtendedJavaWrapper::PsPushNotificationsExtended_Init;
jmethodID FPsPushNotificationsExtendedJavaWrapper::PsPushNotificationsExtended_LocalNotificationScheduleAtTime;
jmethodID FPsPushNotificationsExtendedJavaWrapper::PsPushNotificationsExtended_LocalNotificationAddActionToCategory;
jmethodID FPsPushNotificationsExtendedJavaWrapper::PsPushNotificationsExtended_LocalNotificationClearAll;
jmethodID FPsPushNotificationsExtendedJavaWrapper::PsPushNotificationsExtended_LocalNotificationClearById;
jmethodID FPsPushNotificationsExtendedJavaWrapper::PsPushNotificationsExtended_LastNotificationActivationCode;
jmethodID FPsPushNotificationsExtendedJavaWrapper::PsPushNotificationsExtended_LastNotificationActionId;

#endif // USE_ANDROID_JNI

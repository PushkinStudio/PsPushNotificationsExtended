// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.

#include "PsPushNotificationsExtendedPrivatePCH.h"
#include "PsPushNotificationsExtendedManager.h"
#include "PsPushNotificationsExtendedTypes.h"

#incldue "Android/PsPushNotificationsExtendedJavaWrapper.h"

void UPsPushNotificationsExtendedManager::RequestPushNotifications()
{
	UE_LOG(LogPsPushNotificationsExtended, Log, TEXT("%s called"), *PS_PUSH_FUNC_LINE);

	FPsPushNotificationsExtendedJavaWrapper::Init();
}

void UPsPushNotificationsExtendedManager::AddNotificationCategory(const FString& Name, const TArray<FPsNotificationsAction>& Actions)
{
	UE_LOG(LogPsPushNotificationsExtended, Log, TEXT("%s called"), *PS_PUSH_FUNC_LINE);

	FPsPushNotificationsExtendedJavaWrapper::LocalNotificationAddCategory(Name, Actions);
}

FString UPsPushNotificationsExtendedManager::SendLocalNotificationFromNow(float SecondsFromNow, const FPsNotification& Notification)
{
	FDateTime TargetTime = FDateTime::Now();
	TargetTime += FTimespan::FromSeconds(SecondsFromNow);

	return SendLocalNotification(TargetTime, true, Notification);
}

FString UPsPushNotificationsExtendedManager::SendLocalNotification(const FDateTime& DateTime, bool bLocalTime, const FPsNotification& Notification)
{
	UE_LOG(LogPsPushNotificationsExtended, Log, TEXT("%s called"), *PS_PUSH_FUNC_LINE);

	FString PushIdStr = FPsPushNotificationsExtendedJavaWrapper::LocalNotificationScheduleAtTime(DateTime, bLocalTime, Notification.Title.ToString(), Notification.Body.ToString(), Notification.ActivationCode, Notification.Category, Notification.ContentURL);
	return PushIdStr;
}

void UPsPushNotificationsExtendedManager::ClearAllLocalNotifications()
{
	UE_LOG(LogPsPushNotificationsExtended, Log, TEXT("%s called"), *PS_PUSH_FUNC_LINE);

	FPsPushNotificationsExtendedJavaWrapper::ClearAllNotifications();
}

void UPsPushNotificationsExtendedManager::ClearLocalNotificationsWithId(const TArray<FString>& NotificationsIds)
{
	UE_LOG(LogPsPushNotificationsExtended, Log, TEXT("%s called"), *PS_PUSH_FUNC_LINE);
}

FString UPsPushNotificationsExtendedManager::GetLastNotificationActivationCode()
{
	FString OutValue;
	OutValue = FPsPushNotificationsExtendedJavaWrapper::GetLastNotificationActivationCode();
	UE_LOG(LogPsPushNotificationsExtended, Log, TEXT("%s: \"%s\""), *PS_PUSH_FUNC_LINE, *OutValue);

	return OutValue;
}

FString UPsPushNotificationsExtendedManager::GetLastNotificationActionId()
{
	FString OutValue;
	OutValue = FPsPushNotificationsExtendedJavaWrapper::GetLastNotificationAction();
	UE_LOG(LogPsPushNotificationsExtended, Log, TEXT("%s: \"%s\""), *PS_PUSH_FUNC_LINE, *OutValue);

	return OutValue;
}

void UPsPushNotificationsExtendedManager::ClearLastNotificationData()
{
	// Nothing to do here required for android
}

// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.

#include "PsPushNotificationsExtendedPrivatePCH.h"
#include "PsPushNotificationsExtendedManager.h"
#include "PsPushNotificationsExtendedTypes.h"

void UPsPushNotificationsExtendedManager::RequestPushNotifications()
{
	UE_LOG(LogPsPushNotificationsExtended, Log, TEXT("%s called"), *PS_PUSH_FUNC_LINE);
}

void UPsPushNotificationsExtendedManager::AddNotificationCategory(const FString& Name, const TArray<FPsNotificationsAction>& Actions)
{
	UE_LOG(LogPsPushNotificationsExtended, Log, TEXT("%s called"), *PS_PUSH_FUNC_LINE);
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
	return TEXT("");
}

void UPsPushNotificationsExtendedManager::ClearAllLocalNotifications()
{
	UE_LOG(LogPsPushNotificationsExtended, Log, TEXT("%s called"), *PS_PUSH_FUNC_LINE);
}

void UPsPushNotificationsExtendedManager::ClearLocalNotificationsWithId(const TArray<FString>& NotificationsIds)
{
	UE_LOG(LogPsPushNotificationsExtended, Log, TEXT("%s called"), *PS_PUSH_FUNC_LINE);
}

FString UPsPushNotificationsExtendedManager::GetLastNotificationActivationCode()
{
	return TEXT("");
}

FString UPsPushNotificationsExtendedManager::GetLastNotificationActionId()
{
	return TEXT("");
}

void UPsPushNotificationsExtendedManager::ClearLastNotificationData()
{
}

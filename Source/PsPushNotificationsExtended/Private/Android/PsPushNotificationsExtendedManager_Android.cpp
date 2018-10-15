// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.

#include "PsPushNotificationsExtendedPrivatePCH.h"
#include "PsPushNotificationsExtendedManager_Android.h"
#include "PsPushNotificationsExtendedTypes.h"
#include "Android/PsPushNotificationsExtendedJavaWrapper.h"

UPsPushNotificationsExtendedManager* UPsPushNotificationsExtendedManager::GetInstance()
{
	if (!PushNotificationsExtendedManagerInstance)
	{
		UClass* ManagerClass = UPsPushNotificationsExtendedManagerAndroid::StaticClass();
		PushNotificationsExtendedManagerInstance = NewObject<UPsPushNotificationsExtendedManagerAndroid>(GetTransientPackage(), ManagerClass);
		PushNotificationsExtendedManagerInstance->SetFlags(RF_Standalone);
		PushNotificationsExtendedManagerInstance->AddToRoot();
	}

	return PushNotificationsExtendedManagerInstance;
}

UPsPushNotificationsExtendedManagerAndroid::UPsPushNotificationsExtendedManagerAndroid(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UPsPushNotificationsExtendedManagerAndroid::RequestPushNotifications()
{
	UE_LOG(LogPsPushNotificationsExtended, Log, TEXT("%s called"), *PS_PUSH_FUNC_LINE);

	FPsPushNotificationsExtendedJavaWrapper::Init();
}

void UPsPushNotificationsExtendedManagerAndroid::AddNotificationCategory(const FString& Name, const TArray<FPsNotificationsAction>& Actions)
{
	UE_LOG(LogPsPushNotificationsExtended, Log, TEXT("%s called"), *PS_PUSH_FUNC_LINE);

	FPsPushNotificationsExtendedJavaWrapper::LocalNotificationAddCategory(Name, Actions);
}

FString UPsPushNotificationsExtendedManagerAndroid::SendLocalNotificationFromNow(float SecondsFromNow, const FPsNotification& Notification)
{
	FDateTime TargetTime = FDateTime::Now();
	TargetTime += FTimespan::FromSeconds(SecondsFromNow);

	return SendLocalNotification(TargetTime, true, Notification);
}

FString UPsPushNotificationsExtendedManagerAndroid::SendLocalNotification(const FDateTime& DateTime, bool bLocalTime, const FPsNotification& Notification)
{
	UE_LOG(LogPsPushNotificationsExtended, Log, TEXT("%s called"), *PS_PUSH_FUNC_LINE);

	FString PushIdStr = FPsPushNotificationsExtendedJavaWrapper::LocalNotificationScheduleAtTime(DateTime, bLocalTime, Notification.Title.ToString(), Notification.Body.ToString(), Notification.ActivationCode, Notification.Category, Notification.ContentURL);
	return PushIdStr;
}

void UPsPushNotificationsExtendedManagerAndroid::ClearAllLocalNotifications()
{
	UE_LOG(LogPsPushNotificationsExtended, Log, TEXT("%s called"), *PS_PUSH_FUNC_LINE);

	FPsPushNotificationsExtendedJavaWrapper::ClearAllNotifications();
}

void UPsPushNotificationsExtendedManagerAndroid::ClearLocalNotificationsWithId(const TArray<FString>& NotificationsIds)
{
	for (const FString& NotificationId : NotificationsIds)
	{
		UE_LOG(LogPsPushNotificationsExtended, Log, TEXT("%s called with ID: %s"), *PS_PUSH_FUNC_LINE, *NotificationId);
		FPsPushNotificationsExtendedJavaWrapper::ClearNotificationsById(NotificationId);
	}
}

FString UPsPushNotificationsExtendedManagerAndroid::GetLastNotificationActivationCode()
{
	FString OutValue;
	OutValue = FPsPushNotificationsExtendedJavaWrapper::GetLastNotificationActivationCode();
	UE_LOG(LogPsPushNotificationsExtended, Log, TEXT("%s: \"%s\""), *PS_PUSH_FUNC_LINE, *OutValue);

	return OutValue;
}

FString UPsPushNotificationsExtendedManagerAndroid::GetLastNotificationActionId()
{
	FString OutValue;
	OutValue = FPsPushNotificationsExtendedJavaWrapper::GetLastNotificationAction();
	UE_LOG(LogPsPushNotificationsExtended, Log, TEXT("%s: \"%s\""), *PS_PUSH_FUNC_LINE, *OutValue);

	return OutValue;
}

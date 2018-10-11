// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.

#include "PsPushNotificationsExtendedPrivatePCH.h"
#include "PsPushNotificationsExtendedManager.h"
#include "PsPushNotificationsExtendedTypes.h"

DEFINE_LOG_CATEGORY(LogPsPushNotificationsExtended);

static UPsPushNotificationsExtendedManager* PushNotificationsExtendedManagerInstance = nullptr;

UPsPushNotificationsExtendedManager* UPsPushNotificationsExtendedManager::GetInstance()
{
	if (!PushNotificationsExtendedManagerInstance)
	{
		CreateInstance();
	}

	return PushNotificationsExtendedManagerInstance;
}

void UPsPushNotificationsExtendedManager::CreateInstance()
{
	if (PushNotificationsExtendedManagerInstance)
	{
		UE_LOG(LogPsPushNotificationsExtended, Error, TEXT("%s instance already created"), *PS_PUSH_FUNC_LINE);
		return;
	}

	UClass* ManagerClass = UPsPushNotificationsExtendedManager::StaticClass();
	PushNotificationsExtendedManagerInstance = NewObject<UPsPushNotificationsExtendedManager>(GetTransientPackage(), ManagerClass);
	PushNotificationsExtendedManagerInstance->SetFlags(RF_Standalone);
	PushNotificationsExtendedManagerInstance->AddToRoot();
}

UPsPushNotificationsExtendedManager::UPsPushNotificationsExtendedManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	if (PushNotificationsExtendedManagerInstance != nullptr)
	{
		UE_LOG(LogPsPushNotificationsExtended, Error, TEXT("%s Attempt to create another instance"), *PS_PUSH_FUNC_LINE);
	}

	UE_LOG(LogPsPushNotificationsExtended, Log, TEXT("%s created"), *PS_PUSH_FUNC_LINE);
}

UPsPushNotificationsExtendedManager::~UPsPushNotificationsExtendedManager()
{
	if (PushNotificationsExtendedManagerInstance != this)
	{
		UE_LOG(LogPsPushNotificationsExtended, Warning, TEXT("%s another manager instance have to be destroyed"), *PS_PUSH_FUNC_LINE);
		return;
	}

	PushNotificationsExtendedManagerInstance = nullptr;
}

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

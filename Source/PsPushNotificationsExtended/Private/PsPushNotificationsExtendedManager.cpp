// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.

#include "PsPushNotificationsExtendedPrivatePCH.h"
#include "PsPushNotificationsExtendedManager.h"
#include "PsPushNotificationsExtendedTypes.h"

#if PLATFORM_IOS
#include "IOS/PsPushNotificationsExtended_IOS.h"
#endif // PLATFORM_IOS

DEFINE_LOG_CATEGORY(LogPsPushNotificationsExtended);

static UPsPushNotificationsExtendedManager* PushNotificationsExtendedManagerInstance = nullptr;

UPsPushNotificationsExtendedManager* UPsPushNotificationsExtendedManager::GetInstance()
{
	return PushNotificationsExtendedManagerInstance;
}

void UPsPushNotificationsExtendedManager::CreateInstance()
{
	if (PushNotificationsExtendedManagerInstance)
	{
		UE_LOG(LogPsPushNotificationsExtended, Error, TEXT("UPsPushNotificationsExtendedManager instance already created"));
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
		UE_LOG(LogPsPushNotificationsExtended, Error, TEXT("UPsPushNotificationsExtendedManager Attempt to create another instance"));
	}

	UE_LOG(LogPsPushNotificationsExtended, Log, TEXT("UPsPushNotificationsExtendedManager created"));
}

UPsPushNotificationsExtendedManager::~UPsPushNotificationsExtendedManager() throw()
{
	if (PushNotificationsExtendedManagerInstance != this)
	{
		UE_LOG(LogPsPushNotificationsExtended, Error, TEXT("UPsPushNotificationsExtendedManager another manager instance have to be destroyed"));
		return;
	}

	PushNotificationsExtendedManagerInstance = nullptr;
}

void UPsPushNotificationsExtendedManager::RequestPushNotifications()
{
	UE_LOG(LogPsPushNotificationsExtended, Log, TEXT("UPsPushNotificationsExtendedManager::RequestPushNotifications"));

#if PLATFORM_IOS
	[[PsPushNotificationsExtendedDelegate sharedInstance] requestAuthorization];
#endif // PLATFORM_IOS
}

void UPsPushNotificationsExtendedManager::AddNotificationCategory(const FString& Name, const TArray<FPsNotificationsAction>& Actions)
{
	UE_LOG(LogPsPushNotificationsExtended, Log, TEXT("UPsPushNotificationsExtendedManager::AddNotificationCategory"));

#if PLATFORM_IOS
	NSMutableArray* IOSActions = [[NSMutableArray alloc] init];
	for (const FPsNotificationsAction& Action : Actions)
	{
		PsActionArgIOS* IOSAction = [[PsActionArgIOS alloc] initWithTitle:
			 [NSString stringWithFString: Action.Title] andAction:
			 [NSString stringWithFString: Action.ActionId]];
		[IOSActions addObject: IOSAction];
	}

	[[PsPushNotificationsExtendedDelegate sharedInstance] registerNotificationCategories:
	 	[NSString stringWithFString: Name] andActions: IOSActions];
#endif // PLATFORM_IOS
}

void UPsPushNotificationsExtendedManager::SendLocalNotificationFromNow(float SecondsFromNow, const FPsNotification& Notification)
{
	FDateTime TargetTime = FDateTime::Now();
	TargetTime += FTimespan::FromSeconds(SecondsFromNow);

	SendLocalNotification(TargetTime, true, Notification);
}

void UPsPushNotificationsExtendedManager::SendLocalNotification(const FDateTime& DateTime, bool bLocalTime, const FPsNotification& Notification)
{
	UE_LOG(LogPsPushNotificationsExtended, Log, TEXT("UPsPushNotificationsExtendedManager::SendLocalNotification"));

#if PLATFORM_IOS
	NSDateComponents *dateComps = [[NSDateComponents alloc] init];
	[dateComps setDay : DateTime.GetDay()];
	[dateComps setMonth : DateTime.GetMonth()];
	[dateComps setYear : DateTime.GetYear()];
	[dateComps setHour : DateTime.GetHour()];
	[dateComps setMinute : DateTime.GetMinute()];
	[dateComps setSecond : DateTime.GetSecond()];

	[[PsPushNotificationsExtendedDelegate sharedInstance] scheduleLocalNotificationAtTime: dateComps
		isLocalTime: bLocalTime
		andTitle: [NSString stringWithFString: Notification.Title.ToString()]
		andSubtitle: [NSString stringWithFString: Notification.Subtitle.ToString()]
		andBody: [NSString stringWithFString: Notification.Body.ToString()]
		andAlertAction: nil
		andCategory: [NSString stringWithFString: Notification.Category]
		andImageURL: [NSString stringWithFString: Notification.ContentURL]];

#endif // PLATFORM_IOS
}
	

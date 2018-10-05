// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.

#include "PsPushNotificationsExtendedPrivatePCH.h"
#include "PsPushNotificationsExtendedManager.h"
#include "PsPushNotificationsExtendedTypes.h"

#if PLATFORM_IOS
#include "IOS/PsPushNotificationsExtended_IOS.h"
#endif // PLATFORM_IOS
#if PLATFORM_ANDROID
#include "Android/PsPushNotificationsExtendedJavaWrapper.h"
#endif // PLATFORM_ANDROID


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

UPsPushNotificationsExtendedManager::~UPsPushNotificationsExtendedManager()
{
	if (PushNotificationsExtendedManagerInstance != this)
	{
		UE_LOG(LogPsPushNotificationsExtended, Warning, TEXT("UPsPushNotificationsExtendedManager another manager instance have to be destroyed"));
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
#if PLATFORM_ANDROID
	FPsPushNotificationsExtendedJavaWrapper::Init();
#endif //PLATFORM_ANDROID
}

void UPsPushNotificationsExtendedManager::AddNotificationCategory(const FString& Name, const TArray<FPsNotificationsAction>& Actions)
{
	UE_LOG(LogPsPushNotificationsExtended, Log, TEXT("UPsPushNotificationsExtendedManager::AddNotificationCategory"));

#if PLATFORM_IOS
	NSMutableArray* IOSActions = [[[NSMutableArray alloc] init] autorelease];
	for (const FPsNotificationsAction& Action : Actions)
	{
		PsActionArgIOS* IOSAction = [[[PsActionArgIOS alloc] initWithTitle:
			[NSString stringWithFString: Action.Title] andAction:
			[NSString stringWithFString: Action.ActionId]] autorelease];
		[IOSActions addObject: IOSAction];
	}

	[[PsPushNotificationsExtendedDelegate sharedInstance] registerNotificationCategories:
	 	[NSString stringWithFString: Name] andActions: IOSActions];
#endif // PLATFORM_IOS
#if PLATFORM_ANDROID
	FPsPushNotificationsExtendedJavaWrapper::LocalNotificationAddCategory(Name, Actions);
#endif // PLATFORM_ANDROID
}

FString UPsPushNotificationsExtendedManager::SendLocalNotificationFromNow(float SecondsFromNow, const FPsNotification& Notification)
{
	FDateTime TargetTime = FDateTime::Now();
	TargetTime += FTimespan::FromSeconds(SecondsFromNow);

	return SendLocalNotification(TargetTime, true, Notification);
}

FString UPsPushNotificationsExtendedManager::SendLocalNotification(const FDateTime& DateTime, bool bLocalTime, const FPsNotification& Notification)
{
	UE_LOG(LogPsPushNotificationsExtended, Log, TEXT("UPsPushNotificationsExtendedManager::SendLocalNotification"));

	FString PushIdStr;
#if PLATFORM_IOS
	NSDateComponents *dateComps = [[[NSDateComponents alloc] init] autorelease];
	[dateComps setDay : DateTime.GetDay()];
	[dateComps setMonth : DateTime.GetMonth()];
	[dateComps setYear : DateTime.GetYear()];
	[dateComps setHour : DateTime.GetHour()];
	[dateComps setMinute : DateTime.GetMinute()];
	[dateComps setSecond : DateTime.GetSecond()];

	NSString* PushId = [[PsPushNotificationsExtendedDelegate sharedInstance] scheduleLocalNotificationAtTime: dateComps
		isLocalTime: bLocalTime
		andTitle: [NSString stringWithFString: Notification.Title.ToString()]
		andSubtitle: [NSString stringWithFString: Notification.Subtitle.ToString()]
		andBody: [NSString stringWithFString: Notification.Body.ToString()]
		andAlertAction: nil
		andCategory: [NSString stringWithFString: Notification.Category]
		andImageURL: [NSString stringWithFString: Notification.ContentURL]
		isLocal: Notification.bLocalContent
		andSound:[NSString stringWithFString: Notification.SoundName]
		andBadge: [NSNumber numberWithInteger: (NSInteger)Notification.BadgeNumber]];

	if (PushId)
	{
		PushIdStr = FString(PushId);
	}
#endif // PLATFORM_IOS
#if PLATFORM_ANDROID
	FPsPushNotificationsExtendedJavaWrapper::LocalNotificationScheduleAtTime(DateTime, bLocalTime, Notification.Title.ToString(), Notification.Body.ToString(), TEXT("ActivationEvent"), Notification.Category, Notification.ContentURL);
#endif // PLATFORM_ANDROID
	return PushIdStr;
}

void UPsPushNotificationsExtendedManager::ClearAllLocalNotifications()
{
	UE_LOG(LogPsPushNotificationsExtended, Log, TEXT("UPsPushNotificationsExtendedManager::ClearAllLocalNotifications"));

#if PLATFORM_IOS
	[[PsPushNotificationsExtendedDelegate sharedInstance] clearAllLocalNotifications];
#endif // PLATFORM_IOS
#if PLATFORM_ANDROID
	FPsPushNotificationsExtendedJavaWrapper::ClearAllNotifications();
#endif // PLATFORM_ANDROID
}

void UPsPushNotificationsExtendedManager::ClearLocalNotificationsWithId(const TArray<FString>& NotificationsIds)
{
	UE_LOG(LogPsPushNotificationsExtended, Log, TEXT("UPsPushNotificationsExtendedManager::ClearLocalNotificationsWithId"));

#if PLATFORM_IOS
	NSMutableArray* IOSNotificationsIds = [[[NSMutableArray alloc] init] autorelease];
	for (const FString& NotificationId : NotificationsIds)
	{
		[IOSNotificationsIds addObject: [NSString stringWithFString: NotificationId]];
	}

	[[PsPushNotificationsExtendedDelegate sharedInstance] clearLocalNotificationByIds: IOSNotificationsIds];
#endif // PLATFORM_IOS
}

FString UPsPushNotificationsExtendedManager::GetLastNotificationActionId()
{
	FString OutValue;
#if PLATFORM_IOS
	if ([PsPushNotificationsExtendedDelegate sharedInstance] != nil)
	{
		NSString* lastActionId = [[PsPushNotificationsExtendedDelegate sharedInstance] getLastActionId];
		if (lastActionId)
		{
			OutValue = FString(lastActionId);
		}
	}
#endif // PLATFORM_IOS
#if PLATFORM_ANDROID
	OutValue = FPsPushNotificationsExtendedJavaWrapper::GetLastNotificationAction();
#endif // PLATFORM_ANDROID
	UE_LOG(LogPsPushNotificationsExtended, Log, TEXT("UPsPushNotificationsExtendedManager::GetLastNotificationActionId: \"%s\""), *OutValue);

	return OutValue;
}

void UPsPushNotificationsExtendedManager::ClearLastNotificationData()
{
#if PLATFORM_IOS
	if ([PsPushNotificationsExtendedDelegate sharedInstance] != nil)
	{
		[[PsPushNotificationsExtendedDelegate sharedInstance] clearNotificationDictionaryData];
	}
#endif // PLATFORM_IOS
}

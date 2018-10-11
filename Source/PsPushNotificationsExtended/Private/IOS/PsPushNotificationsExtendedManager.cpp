// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.

#include "PsPushNotificationsExtendedPrivatePCH.h"
#include "PsPushNotificationsExtendedManager.h"
#include "PsPushNotificationsExtendedTypes.h"

#include "IOS/PsPushNotificationsExtended_IOS.h"

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

	[[PsPushNotificationsExtendedDelegate sharedInstance] requestAuthorization];
}

void UPsPushNotificationsExtendedManager::AddNotificationCategory(const FString& Name, const TArray<FPsNotificationsAction>& Actions)
{
	UE_LOG(LogPsPushNotificationsExtended, Log, TEXT("%s called"), *PS_PUSH_FUNC_LINE);

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

	FString PushIdStr;
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
		andBadge: [NSNumber numberWithInteger: (NSInteger)Notification.BadgeNumber]
		andActivationCode: [NSString stringWithFString: Notification.ActivationCode]];

	if (PushId)
	{
		PushIdStr = FString(PushId);
	}
	return PushIdStr;
}

void UPsPushNotificationsExtendedManager::ClearAllLocalNotifications()
{
	UE_LOG(LogPsPushNotificationsExtended, Log, TEXT("%s called"), *PS_PUSH_FUNC_LINE);

	[[PsPushNotificationsExtendedDelegate sharedInstance] clearAllLocalNotifications];
}

void UPsPushNotificationsExtendedManager::ClearLocalNotificationsWithId(const TArray<FString>& NotificationsIds)
{
	UE_LOG(LogPsPushNotificationsExtended, Log, TEXT("%s called"), *PS_PUSH_FUNC_LINE);

	NSMutableArray* IOSNotificationsIds = [[[NSMutableArray alloc] init] autorelease];
	for (const FString& NotificationId : NotificationsIds)
	{
		[IOSNotificationsIds addObject: [NSString stringWithFString: NotificationId]];
	}

	[[PsPushNotificationsExtendedDelegate sharedInstance] clearLocalNotificationByIds: IOSNotificationsIds];
}

FString UPsPushNotificationsExtendedManager::GetLastNotificationActivationCode()
{
	FString OutValue;
	if ([PsPushNotificationsExtendedDelegate sharedInstance] != nil)
	{
		NSString* lastCode = [[PsPushNotificationsExtendedDelegate sharedInstance] getLastActivationCodeFromDictionary];
		if (lastCode)
		{
			OutValue = FString(lastCode);
		}
	}
	UE_LOG(LogPsPushNotificationsExtended, Log, TEXT("%s: \"%s\""), *PS_PUSH_FUNC_LINE, *OutValue);

	return OutValue;
}

FString UPsPushNotificationsExtendedManager::GetLastNotificationActionId()
{
	FString OutValue;

	if ([PsPushNotificationsExtendedDelegate sharedInstance] != nil)
	{
		NSString* lastActionId = [[PsPushNotificationsExtendedDelegate sharedInstance] getLastActionIdFromDictionary];
		if (lastActionId)
		{
			OutValue = FString(lastActionId);
		}
	}
	UE_LOG(LogPsPushNotificationsExtended, Log, TEXT("%s: \"%s\""), *PS_PUSH_FUNC_LINE, *OutValue);

	return OutValue;
}

void UPsPushNotificationsExtendedManager::ClearLastNotificationData()
{
	if ([PsPushNotificationsExtendedDelegate sharedInstance] != nil)
	{
		[[PsPushNotificationsExtendedDelegate sharedInstance] clearNotificationDictionaryData];
	}
}

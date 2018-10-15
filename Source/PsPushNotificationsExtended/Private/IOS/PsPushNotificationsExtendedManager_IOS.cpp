// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.

#include "PsPushNotificationsExtendedPrivatePCH.h"
#include "PsPushNotificationsExtendedManager_IOS.h"
#include "PsPushNotificationsExtendedTypes.h"
#include "IOS/PsPushNotificationsExtended_IOS.h"
#include "Misc/CoreDelegates.h"

UPsPushNotificationsExtendedManager* UPsPushNotificationsExtendedManager::GetInstance()
{
	if (!PushNotificationsExtendedManagerInstance)
	{
		UClass* ManagerClass = UPsPushNotificationsExtendedManagerIOS::StaticClass();
		PushNotificationsExtendedManagerInstance = NewObject<UPsPushNotificationsExtendedManagerIOS>(GetTransientPackage(), ManagerClass);
		PushNotificationsExtendedManagerInstance->SetFlags(RF_Standalone);
		PushNotificationsExtendedManagerInstance->AddToRoot();
	}

	return PushNotificationsExtendedManagerInstance;
}

UPsPushNotificationsExtendedManagerIOS::UPsPushNotificationsExtendedManagerIOS(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FCoreDelegates::ApplicationHasEnteredForegroundDelegate.AddUObject(this, &UPsPushNotificationsExtendedManagerIOS::OnApplicationHasEnteredForegroundDelegate);
}

UPsPushNotificationsExtendedManagerIOS::~UPsPushNotificationsExtendedManagerIOS()
{
	FCoreDelegates::ApplicationHasEnteredForegroundDelegate.RemoveAll(this);
}

void UPsPushNotificationsExtendedManagerIOS::RequestPushNotifications()
{
	UE_LOG(LogPsPushNotificationsExtended, Log, TEXT("%s called"), *PS_PUSH_FUNC_LINE);

	[[PsPushNotificationsExtendedDelegate sharedInstance] requestAuthorization];
}

void UPsPushNotificationsExtendedManagerIOS::AddNotificationCategory(const FString& Name, const TArray<FPsNotificationsAction>& Actions)
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

FString UPsPushNotificationsExtendedManagerIOS::SendLocalNotificationFromNow(float SecondsFromNow, const FPsNotification& Notification)
{
	FDateTime TargetTime = FDateTime::Now();
	TargetTime += FTimespan::FromSeconds(SecondsFromNow);

	return SendLocalNotification(TargetTime, true, Notification);
}

FString UPsPushNotificationsExtendedManagerIOS::SendLocalNotification(const FDateTime& DateTime, bool bLocalTime, const FPsNotification& Notification)
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

void UPsPushNotificationsExtendedManagerIOS::ClearAllLocalNotifications()
{
	UE_LOG(LogPsPushNotificationsExtended, Log, TEXT("%s called"), *PS_PUSH_FUNC_LINE);

	[[PsPushNotificationsExtendedDelegate sharedInstance] clearAllLocalNotifications];
}

void UPsPushNotificationsExtendedManagerIOS::ClearLocalNotificationsWithId(const TArray<FString>& NotificationsIds)
{
	UE_LOG(LogPsPushNotificationsExtended, Log, TEXT("%s called"), *PS_PUSH_FUNC_LINE);

	NSMutableArray* IOSNotificationsIds = [[[NSMutableArray alloc] init] autorelease];
	for (const FString& NotificationId : NotificationsIds)
	{
		[IOSNotificationsIds addObject: [NSString stringWithFString: NotificationId]];
	}

	[[PsPushNotificationsExtendedDelegate sharedInstance] clearLocalNotificationByIds: IOSNotificationsIds];
}

FString UPsPushNotificationsExtendedManagerIOS::GetLastNotificationActivationCode()
{
	FString OutValue;
	if ([PsPushNotificationsExtendedDelegate sharedInstance] != nil)
	{
		NSString* lastCode = [[PsPushNotificationsExtendedDelegate sharedInstance] currentActivationCode];
		if (lastCode)
		{
			OutValue = FString(lastCode);
		}
	}
	UE_LOG(LogPsPushNotificationsExtended, Log, TEXT("%s: \"%s\""), *PS_PUSH_FUNC_LINE, *OutValue);

	return OutValue;
}

FString UPsPushNotificationsExtendedManagerIOS::GetLastNotificationActionId()
{
	FString OutValue;

	if ([PsPushNotificationsExtendedDelegate sharedInstance] != nil)
	{
		NSString* lastActionId = [[PsPushNotificationsExtendedDelegate sharedInstance] currentActionId];
		if (lastActionId)
		{
			OutValue = FString(lastActionId);
		}
	}
	UE_LOG(LogPsPushNotificationsExtended, Log, TEXT("%s: \"%s\""), *PS_PUSH_FUNC_LINE, *OutValue);

	return OutValue;
}

void UPsPushNotificationsExtendedManagerIOS::OnApplicationHasEnteredForegroundDelegate()
{
	PsPushNotificationsExtendedDelegate* iosPushDelegate = [PsPushNotificationsExtendedDelegate sharedInstance];
	if (iosPushDelegate == nil)
	{
		return;
	}

	iosPushDelegate.currentActivationCode = [iosPushDelegate getLastActivationCodeFromDictionary];
	iosPushDelegate.currentActionId = [iosPushDelegate getLastActionIdFromDictionary];
	[iosPushDelegate clearNotificationDictionaryData];
}

// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.

#include "PsPushNotificationsExtendedPrivatePCH.h"
#include "PsPushNotificationsExtendedManager_Windows.h"
#include "PsPushNotificationsExtendedTypes.h"

UPsPushNotificationsExtendedManager* UPsPushNotificationsExtendedManager::GetInstance()
{
	if (!PushNotificationsExtendedManagerInstance)
	{
		UClass* ManagerClass = UPsPushNotificationsExtendedManagerWindows::StaticClass();
		PushNotificationsExtendedManagerInstance = NewObject<UPsPushNotificationsExtendedManagerWindows>(GetTransientPackage(), ManagerClass);
		PushNotificationsExtendedManagerInstance->SetFlags(RF_Standalone);
		PushNotificationsExtendedManagerInstance->AddToRoot();
	}

	return PushNotificationsExtendedManagerInstance;
}

UPsPushNotificationsExtendedManagerWindows::UPsPushNotificationsExtendedManagerWindows(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

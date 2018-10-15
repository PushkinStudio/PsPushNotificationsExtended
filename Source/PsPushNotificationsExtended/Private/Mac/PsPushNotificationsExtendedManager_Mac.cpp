// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.

#include "PsPushNotificationsExtendedPrivatePCH.h"
#include "PsPushNotificationsExtendedManager_Mac.h"
#include "PsPushNotificationsExtendedTypes.h"

UPsPushNotificationsExtendedManager* UPsPushNotificationsExtendedManager::GetInstance()
{
	if (!PushNotificationsExtendedManagerInstance)
	{
		UClass* ManagerClass = UPsPushNotificationsExtendedManagerMac::StaticClass();
		PushNotificationsExtendedManagerInstance = NewObject<UPsPushNotificationsExtendedManagerMac>(GetTransientPackage(), ManagerClass);
		PushNotificationsExtendedManagerInstance->SetFlags(RF_Standalone);
		PushNotificationsExtendedManagerInstance->AddToRoot();
	}

	return PushNotificationsExtendedManagerInstance;
}

UPsPushNotificationsExtendedManagerMac::UPsPushNotificationsExtendedManagerMac(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

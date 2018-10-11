// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.

#include "PsPushNotificationsExtendedPrivatePCH.h"
#include "PsPushNotificationsExtendedManager.h"
#include "PsPushNotificationsExtendedTypes.h"

DEFINE_LOG_CATEGORY(LogPsPushNotificationsExtended);

static UPsPushNotificationsExtendedManager* PushNotificationsExtendedManagerInstance = nullptr;

UPsPushNotificationsExtendedManager* UPsPushNotificationsExtendedManager::GetPsPushNotificationsExtendedManager()
{
	return GetInstance();
}

UPsPushNotificationsExtendedManager* UPsPushNotificationsExtendedManager::GetInstance()
{
	if (!PushNotificationsExtendedManagerInstance)
	{
		UClass* ManagerClass = UPsPushNotificationsExtendedManager::StaticClass();
		PushNotificationsExtendedManagerInstance = NewObject<UPsPushNotificationsExtendedManager>(GetTransientPackage(), ManagerClass);
		PushNotificationsExtendedManagerInstance->SetFlags(RF_Standalone);
		PushNotificationsExtendedManagerInstance->AddToRoot();
	}

	return PushNotificationsExtendedManagerInstance;
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


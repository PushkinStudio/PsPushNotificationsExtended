// Copyright 2018 Mail.Ru Group. All Rights Reserved.

#include "PsPushNotificationsExtendedPrivatePCH.h"
#include "PsPushNotificationsExtended.h"
#include "PsPushNotificationsExtendedManager.h"
#include "UObject/UObjectGlobals.h"

void FPsPushNotificationsExtended::StartupModule()
{
	// Creating Push manager on startup
//	UClass* ManagerClass = UPsPushNotificationsExtendedManager::StaticClass();
//	Manager = NewObject<UPsPushNotificationsExtendedManager>(GetTransientPackage(), ManagerClass);
//	Manager->SetFlags(RF_Standalone);
//	Manager->AddToRoot();
	UPsPushNotificationsExtendedManager::CreateInstance();
	Manager = UPsPushNotificationsExtendedManager::GetInstance();
}

void FPsPushNotificationsExtended::ShutdownModule()
{
	// Shuttin down Push manager
	if (!GExitPurge)
	{
		// if GExitPurge - manager is already removed
		Manager->RemoveFromRoot();
	}

	Manager = nullptr;
}

IMPLEMENT_MODULE(FPsPushNotificationsExtended, PsPushNotificationsExtended)

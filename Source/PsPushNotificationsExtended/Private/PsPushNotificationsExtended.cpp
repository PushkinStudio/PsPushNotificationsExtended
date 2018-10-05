// Copyright 2018 Mail.Ru Group. All Rights Reserved.

#include "PsPushNotificationsExtendedPrivatePCH.h"
#include "PsPushNotificationsExtended.h"
#include "PsPushNotificationsExtendedManager.h"
#include "UObject/UObjectGlobals.h"

void FPsPushNotificationsExtended::StartupModule()
{
	// Creating Push manager on startup
}

void FPsPushNotificationsExtended::ShutdownModule()
{
	// Shuttin down Push manager
	if (!GExitPurge && UPsPushNotificationsExtendedManager::GetInstance())
	{
		// if GExitPurge - manager is already removed
		UPsPushNotificationsExtendedManager::GetInstance()->RemoveFromRoot();
	}
}

IMPLEMENT_MODULE(FPsPushNotificationsExtended, PsPushNotificationsExtended)

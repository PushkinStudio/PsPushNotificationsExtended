// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.
#pragma once

#include "CoreTypes.h"
#include "Delegates/Delegate.h"
#include "PsPushNotificationsExtendedTypes.h"
#include "PsPushNotificationsExtendedManager.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogPsPushNotificationsExtended, Log, All);

// Action perform delegate type declaration
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPsOnActionPerformDelegate, FString, ActionId, FString, PushData);

UCLASS()
class UPsPushNotificationsExtendedManager : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	// Called when user activates some action from push notifications
	UPROPERTY(BlueprintAssignable)
	FPsOnActionPerformDelegate OnActionPerform;

	UFUNCTION(BlueprintCallable)
	static UPsPushNotificationsExtendedManager* GetInstance();

	UFUNCTION(BlueprintCallable)
	static void CreateInstance();

	~UPsPushNotificationsExtendedManager() throw() override;

	UFUNCTION(BlueprintCallable)
	void RequestPushNotifications();

	UFUNCTION(BlueprintCallable)
	void AddNotificationCategory(const FString& Name, const TArray<FPsNotificationsAction>& Actions);

	UFUNCTION(BlueprintCallable)
	void SendLocalNotificationFromNow(float SecondsFromNow, const FPsNotification& Notification);

	UFUNCTION(BlueprintCallable)
	void SendLocalNotification(const FDateTime& DateTime, bool bLocalTime, const FPsNotification& Notification);
};

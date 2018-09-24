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
	/// Called when user activates some action from push notifications
	UPROPERTY(BlueprintAssignable)
	FPsOnActionPerformDelegate OnActionPerform;

	/// Get instance of the manager
	UFUNCTION(BlueprintCallable)
	static UPsPushNotificationsExtendedManager* GetInstance();

	/// Creates an instance of the class
	UFUNCTION(BlueprintCallable)
	static void CreateInstance();

	/// Destructor method
	~UPsPushNotificationsExtendedManager() throw() override;

	/// Requests authority for push notifications
	UFUNCTION(BlueprintCallable)
	void RequestPushNotifications();

	/// Adds notifications cathegory
	UFUNCTION(BlueprintCallable)
	void AddNotificationCategory(const FString& Name, const TArray<FPsNotificationsAction>& Actions);

	/** Sends local notifications with a specified content in a period of time in seconds
	 @return Notification ID
	 */
	UFUNCTION(BlueprintCallable)
	FString SendLocalNotificationFromNow(float SecondsFromNow, const FPsNotification& Notification);

	/** Sends local notifications with a specified conted at specified time
	 @return Notification ID
	 */
	UFUNCTION(BlueprintCallable)
	FString SendLocalNotification(const FDateTime& DateTime, bool bLocalTime, const FPsNotification& Notification);

	/// Cleares all notifications
	UFUNCTION(BlueprintCallable)
	void ClearAllLocalNotifications();

	/// Removes local notification with specified ID
	UFUNCTION(BlueprintCallable)
	void ClearLocalNotificationsWithId(const TArray<FString>& NotificationsIds);

	/// Gets last notification action id (use at start of the application to navigate user action)
	UFUNCTION(BlueprintCallable)
	FString GetLastNotificationActionId();
};

// Copyright 2018 Mail.Ru Group. All Rights Reserved.

#include "PsPushNotificationsExtendedPrivatePCH.h"

#if PLATFORM_IOS

#include "IOSAppDelegate+PsPushNotificationsExtended.h"
#include "PsPushNotificationsExtended_IOS.h"
#import <Foundation/Foundation.h>
#import <UserNotifications/UserNotifications.h>

@implementation IOSAppDelegate (PsPushNotificationsExtended)

- (BOOL)application:(UIApplication *)application
willFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
	NSLog(@"%s IOSAppDelegate+PsPushNotifications %@", __PRETTY_FUNCTION__, launchOptions);

	self.bDeviceInPortraitMode = false;

	// Creating delegate early to earn push actions
	PsPushNotificationsExtendedDelegate* notificationsDelegate = [PsPushNotificationsExtendedDelegate sharedInstance];
	UNUserNotificationCenter *center = [UNUserNotificationCenter currentNotificationCenter];
	center.delegate = notificationsDelegate;

	return YES;
}

@end

#endif // PLATFORM_IOS

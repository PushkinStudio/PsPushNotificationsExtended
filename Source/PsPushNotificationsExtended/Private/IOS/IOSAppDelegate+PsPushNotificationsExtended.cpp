// Copyright 2018 Mail.Ru Group. All Rights Reserved.

#include "PsPushNotificationsExtendedPrivatePCH.h"

#if PLATFORM_IOS

#include "IOSAppDelegate+PsPushNotificationsExtended.h"
#include "PsPushNotificationsExtended_IOS.h"
#import <Foundation/Foundation.h>

@implementation IOSAppDelegate (PsPushNotifications)

- (BOOL)application:(UIApplication *)application
willFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
	NSLog(@"%s IOSAppDelegate+PsPushNotifications %@", __PRETTY_FUNCTION__, launchOptions);

	self.bDeviceInPortraitMode = false;

	// Creating delegate early to earn push actions
	[PsPushNotificationsExtendedDelegate sharedInstance];

	return YES;
}

@end

#endif // PLATFORM_IOS

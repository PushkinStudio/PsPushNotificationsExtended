// Copyright 2018 Mail.Ru Group. All Rights Reserved.
#pragma once

#if PLATFORM_IOS

#import <UIKit/UIKit.h>
#import <UserNotifications/UserNotifications.h>

// Simple structure to pass action argument as pair
@interface PsActionArgIOS : NSObject
@property (nonatomic, strong) NSString* title;
@property (nonatomic, strong) NSString* actionId;

-(id) initWithTitle: (NSString*) title_ andAction: (NSString*) actionId_;
@end

// IOS interface for RICH user notifications
@interface PsPushNotificationsExtendedDelegate : UIResponder <UNUserNotificationCenterDelegate>

// Get shared instance of delegate
+(id) sharedInstance;

// Request push notifications authorization on IOS
-(void) requestAuthorization;

// Schedule local notification on IOS
-(void) scheduleLocalNotificationAtTime: (NSDateComponents*) dateComponents isLocalTime: (bool) bLocal andTitle: (NSString*) title andSubtitle: (NSString*) subtitle andBody: (NSString*) body andAlertAction: (NSString*) action andCategory: (NSString*) category andImageURL: (NSString*) imageURL;

// Register IOS notification category
-(void) registerNotificationCategories: (NSString*) categoryName andActions: (NSArray<PsActionArgIOS*>*) actions;

@end

#endif // PLATFORM_IOS

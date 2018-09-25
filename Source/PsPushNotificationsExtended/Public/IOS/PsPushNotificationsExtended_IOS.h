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

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

extern NSString* PsNotificationsDataFileName;
extern NSString* PsNotificationActionFieldName;

// IOS interface for RICH user notifications
@interface PsPushNotificationsExtendedDelegate : UIResponder <UNUserNotificationCenterDelegate>

// Get shared instance of delegate
+(id) sharedInstance;

// Request push notifications authorization on IOS
-(void) requestAuthorization;

// Schedule local notification on IOS
-(NSString*) scheduleLocalNotificationAtTime: (NSDateComponents*) dateComponents isLocalTime: (bool) bLocal andTitle: (NSString*) title andSubtitle: (NSString*) subtitle andBody: (NSString*) body andAlertAction: (NSString*) action andCategory: (NSString*) category andImageURL: (NSString*) imageURL andSound: (NSString*) soundName andBadge: (NSNumber*) badgeNumber;

// Register IOS notification category
-(void) registerNotificationCategories: (NSString*) categoryName andActions: (NSArray<PsActionArgIOS*>*) actions;

// Clear all local notifications
-(void) clearAllLocalNotifications;

// Clear notifications by ID
-(void) clearLocalNotificationByIds: (NSArray<NSString*>*) ids;

// Gets string value from temporary saved dictionary
-(NSString*) getLastActionId;

// Save user content dictionary to a temporary file
-(void) saveToTemporaryFile: (NSDictionary*) dictionaryData;

// Load dictionary data from a temporary file
-(NSDictionary*) loadDictionaryFromTemporaryFile;

// Clear saved notification data for the device
-(void) clearNotificationDictionaryData;

@end

#endif // PLATFORM_IOS

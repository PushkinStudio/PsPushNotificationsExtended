// Copyright 2018 Mail.Ru Group. All Rights Reserved.
#pragma once

#if PLATFORM_IOS

#import <UIKit/UIKit.h>
#import <UserNotifications/UserNotifications.h>
#import <Foundation/Foundation.h>

/** Simple structure to pass action argument as pair */
@interface PsActionArgIOS : NSObject
@property (nonatomic, strong) NSString* title;
@property (nonatomic, strong) NSString* actionId;

-(id) initWithTitle: (NSString*) title andAction: (NSString*) actionId;

@end

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

extern NSString* PsNotificationsDataFileName;
extern NSString* PsNotificationActionFieldName;
extern NSString* PsNotificationaAtivationCodeFieldName;

/** IOS interface for RICH user notifications */
@interface PsPushNotificationsExtendedDelegate : NSObject <UNUserNotificationCenterDelegate>

/** Get shared instance of delegate */
+(id) sharedInstance;

/** Init method averride */
-(id) init;

/** Request push notifications authorization on IOS */
-(void) requestAuthorization;

-(UNNotificationAttachment*) getAttachementWithId:(NSString*) attachementId andURL:(NSString*) fileURL;

/** Create notifications content */
-(UNMutableNotificationContent*) notificationContentWith: (NSString*) title andSubtitle:(NSString*) subtitle andBody:(NSString*)body andSound:(NSString*)soundName andBadge: (NSNumber*) badgeNumber andActivationCode:(NSString*) activationCode andCategory:(NSString*) category;

/** Schedule local notification on IOS */
-(NSString*) scheduleLocalNotificationAtTime: (NSDateComponents*) dateComponents isLocalTime: (bool) bLocal andTitle: (NSString*) title andSubtitle: (NSString*) subtitle andBody: (NSString*) body andAlertAction: (NSString*) action andCategory: (NSString*) category andImageURL: (NSString*) imageURL isLocal: (bool) bIsLocalResource andSound: (NSString*) soundName andBadge: (NSNumber*) badgeNumber andActivationCode: (NSString*) activationCode;

/** Schedule local notification */
-(void) scheduleLocalPushRequestWithId: (NSString*) pushId atTime: (NSDateComponents*) dateComponents andContent: (UNMutableNotificationContent*) content;

/** Register IOS notification category */
-(void) registerNotificationCategories: (NSString*) categoryName andActions: (NSArray<PsActionArgIOS*>*) actions;

/** Clear all local notifications */
-(void) clearAllLocalNotifications;

/** Clear notifications by ID */
-(void) clearLocalNotificationByIds: (NSArray<NSString*>*) ids;

/** Gets string value of activation code from temporary saved dictionary */
-(NSString*) getLastActivationCodeFromDictionary;

/** Gets string value from temporary saved dictionary */
-(NSString*) getLastActionIdFromDictionary;

/** Save user content dictionary to a temporary file */
-(void) saveToTemporaryFile: (NSDictionary*) dictionaryData;

/** Load dictionary data from a temporary file */
-(NSDictionary*) loadDictionaryFromTemporaryFile;

/** Clear saved notification data for the device */
-(void) clearNotificationDictionaryData;

@end

#endif // PLATFORM_IOS

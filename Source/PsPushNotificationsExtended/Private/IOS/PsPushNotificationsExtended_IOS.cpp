// Copyright 2018 Mail.Ru Group. All Rights Reserved.

#include "PsPushNotificationsExtendedPrivatePCH.h"
#include "PsPushNotificationsExtended_IOS.h"
#include "Misc/CoreDelegates.h"
#include "PsPushNotificationsExtendedManager.h"
#include "IOSAppDelegate+PsPushNotificationsExtended.h"

#import <Foundation/Foundation.h>

#if PLATFORM_IOS

@implementation PsActionArgIOS

@synthesize title;
@synthesize actionId;

-(id) initWithTitle: (NSString*) actionTitle andAction: (NSString*) actId
{
	self = [super init];
	if (self)
	{
		self.title = actionTitle;
		self.actionId = actId;
	}
	return self;
}

@end

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

NSString* PsNotificationsDataFileName = @"local_notification.action";
NSString* PsNotificationActionFieldName = @"Action";
NSString* PsNotificationaAtivationCodeFieldName = @"ActivationCode";

@implementation PsPushNotificationsExtendedDelegate

+(id) sharedInstance
{
	static PsPushNotificationsExtendedDelegate *Instance = nil;
	static dispatch_once_t onceToken;
	dispatch_once(&onceToken, ^{
		Instance = [[self alloc] init];
		NSLog(@"PsPushNotificationsExtendedDelegate instance created");
	});
	return Instance;
}

-(id) init
{
	self = [super init];
	if (self)
	{
		UNUserNotificationCenter *center = [UNUserNotificationCenter currentNotificationCenter];
		center.delegate = self;
		NSLog(@"PsPushNotificationsExtendedDelegate init delegate");
	}

	return self;
}

-(void) requestAuthorization
{
	if (@available(iOS 10, *))
	{
		NSLog(@"PsPushNotificationsExtendedDelegate request authorization");

		__block UNUserNotificationCenter *center = [UNUserNotificationCenter currentNotificationCenter];
		[center requestAuthorizationWithOptions:(UNAuthorizationOptionSound | UNAuthorizationOptionAlert | UNAuthorizationOptionBadge) completionHandler:^(BOOL granted, NSError * _Nullable error){
			if(!error)
			{
				[[UIApplication sharedApplication] registerForRemoteNotifications];

				NSLog(@"PsPushNotificationsExtendedDelegate requestAuthorizationWithOptions success");

				int32 types = (int32)(UNAuthorizationOptionSound | UNAuthorizationOptionAlert | UNAuthorizationOptionBadge);
				FFunctionGraphTask::CreateAndDispatchWhenReady([types]()
				{
					FCoreDelegates::ApplicationRegisteredForUserNotificationsDelegate.Broadcast(types);
				}, TStatId(), NULL, ENamedThreads::GameThread);
			}
		}];
	}
}

-(UNMutableNotificationContent*) notificationContentWith: (NSString*) title andSubtitle:(NSString*) subtitle andBody:(NSString*)body andSound:(NSString*)soundName andBadge: (NSNumber*) badgeNumber andActivationCode:(NSString*) activationCode andCategory:(NSString*) category
{
	UNMutableNotificationContent* Content = [[[UNMutableNotificationContent alloc] init] autorelease];
	Content.title = title;
	Content.subtitle = subtitle;
	Content.body = body;

	if (soundName && [soundName length] > 0)
	{
		Content.sound = [UNNotificationSound soundNamed: soundName];
	}
	else
	{
		Content.sound = UNNotificationSound.defaultSound;
	}
	if (badgeNumber && [badgeNumber intValue] > 0)
	{
		Content.badge = badgeNumber;
	}
	if (activationCode && [activationCode length] > 0)
	{
		Content.userInfo = [[[NSDictionary alloc] initWithObjectsAndKeys: activationCode, PsNotificationaAtivationCodeFieldName, nil] autorelease];
	}

	Content.categoryIdentifier = category;

	return Content;
}

-(NSString*) scheduleLocalNotificationAtTime: (NSDateComponents*) dateComponents isLocalTime: (bool) bLocal andTitle: (NSString*) title andSubtitle: (NSString*) subtitle andBody: (NSString*) body andAlertAction: (NSString*) action  andCategory: (NSString*) category andImageURL: (NSString*) imageURL isLocal: (bool) bIsLocalResource andSound: (NSString*) soundName andBadge: (NSNumber*) badgeNumber andActivationCode: (NSString*) activationCode
{
	if (@available(iOS 10, *))
	{
		NSString* PushID = [NSUUID UUID].UUIDString;
		UNUserNotificationCenter *center = [UNUserNotificationCenter currentNotificationCenter];
		center.delegate = self;

		// load image content
		NSString *identifier = @"image.jpg";
		NSURL *url = [NSURL URLWithString:imageURL];
		if (url)
		{
			identifier = [NSString stringWithFormat:@"file.%@.%@", PushID, url.lastPathComponent];
		}

		if (bIsLocalResource || !url)
		{
			UNMutableNotificationContent* Content = [self notificationContentWith: title andSubtitle: subtitle andBody: body andSound:soundName andBadge:  badgeNumber andActivationCode: activationCode andCategory: category];

			if (url)
			{
				NSError *localError = nil;
				UNNotificationAttachment *attachment = [UNNotificationAttachment attachmentWithIdentifier:identifier URL:[NSURL fileURLWithPath:imageURL] options:nil error:&localError];
				if(!localError)
				{
					Content.attachments = [NSArray arrayWithObjects: attachment, nil];
					NSLog(@"scheduleLocalNotificationAtTime local attachement assign");
				}
				else
				{
					NSLog(@"scheduleLocalNotificationAtTime error %@", localError);
				}
			}

			[self scheduleLocalPushRequestWithId: PushID atTime: dateComponents andContent: Content];
		}
		else
		{
			NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
			NSString *fileURLPath = [paths objectAtIndex : 0];
			fileURLPath = [fileURLPath stringByAppendingPathComponent: @"NotificationsContent"];

			BOOL isDir = NO;
			if (![[NSFileManager defaultManager] fileExistsAtPath:fileURLPath isDirectory:&isDir] && isDir == NO)
			{
				NSLog(@"scheduleLocalNotificationAtTime creating notifications cache directory: %@", fileURLPath);
				NSError *createCacheDirError = nil;
				[[NSFileManager defaultManager] createDirectoryAtPath: fileURLPath withIntermediateDirectories: YES attributes: nil error: &createCacheDirError];

				if (createCacheDirError)
				{
					NSLog(@"scheduleLocalNotificationAtTime %@", createCacheDirError);
					UNMutableNotificationContent* Content = [self notificationContentWith: title andSubtitle: subtitle andBody: body andSound:soundName andBadge:  badgeNumber andActivationCode: activationCode andCategory: category];
					[self scheduleLocalPushRequestWithId: PushID atTime: dateComponents andContent: Content];
					return PushID;
				}
			}

			NSLog(@"scheduleLocalNotificationAtTime notifications cache directory at: %@", fileURLPath);
			NSString *fileURL = [fileURLPath stringByAppendingPathComponent: identifier];

			NSLog(@"scheduleLocalNotificationAtTime remote content download");
			NSURLSessionDownloadTask *task = [[NSURLSession sharedSession] downloadTaskWithURL: url
				completionHandler:^(NSURL *location, NSURLResponse *response, NSError *error) {

				UNMutableNotificationContent* AsyncContent = [self notificationContentWith: title andSubtitle: subtitle andBody: body andSound:soundName andBadge:  badgeNumber andActivationCode: activationCode andCategory: category];

				NSLog(@"scheduleLocalNotificationAtTime downloadTaskWithURL %@", location);
				if (error)
				{
					NSLog(@"scheduleLocalNotificationAtTime error %@", error);
					[self scheduleLocalPushRequestWithId: PushID atTime: dateComponents andContent: AsyncContent];
					return;
				}

				if (!location)
				{
					NSLog(@"scheduleLocalNotificationAtTime location is nil");
					[self scheduleLocalPushRequestWithId: PushID atTime: dateComponents andContent: AsyncContent];
					return;
				}

				NSError *localError = nil;

				NSString* directory = [fileURL stringByDeletingLastPathComponent];
				NSLog(@"scheduleLocalNotificationAtTime try create dir %@", directory);

				NSLog(@"scheduleLocalNotificationAtTime moving tmp file: %@ to path: %@", [location path], fileURL);
				[[NSFileManager defaultManager] moveItemAtPath: [location path] toPath: fileURL error: &localError];
				if(localError)
				{
					NSLog(@"scheduleLocalNotificationAtTime can not move file: %@", localError);
					[self scheduleLocalPushRequestWithId: PushID atTime: dateComponents andContent: AsyncContent];
					return;
				}

				// Creating attachement
				NSLog(@"scheduleLocalNotificationAtTime setting up notification attachement");
				UNNotificationAttachment *attachment = [UNNotificationAttachment attachmentWithIdentifier:identifier URL:[NSURL fileURLWithPath: fileURL] options:nil error: &localError];
				if (localError)
				{
					NSLog(@"scheduleLocalNotificationAtTime remote attachement assign error %@", localError);
					[self scheduleLocalPushRequestWithId: PushID atTime: dateComponents andContent: AsyncContent];
					return;
				}

				NSLog(@"scheduleLocalNotificationAtTime assigning attachement as notification content");
				AsyncContent.attachments = [NSArray arrayWithObjects: attachment, nil];
				[self scheduleLocalPushRequestWithId: PushID atTime: dateComponents andContent: AsyncContent];

				NSLog(@"scheduleLocalNotificationAtTime remote attachement assign success");
			}];

			[task resume];
		}

		return PushID;
	}

	return nil;
}

-(void) scheduleLocalPushRequestWithId: (NSString*) pushId atTime: (NSDateComponents*) dateComponents andContent: (UNMutableNotificationContent*) content
{
	UNUserNotificationCenter *center = [UNUserNotificationCenter currentNotificationCenter];

	NSCalendar *calendar = [NSCalendar autoupdatingCurrentCalendar];
	UNCalendarNotificationTrigger* trigger = [UNCalendarNotificationTrigger
		triggerWithDateMatchingComponents:dateComponents repeats:NO];

	UNNotificationRequest* Request = [UNNotificationRequest requestWithIdentifier: pushId
		content: content
		trigger: trigger];

	[center addNotificationRequest: Request withCompletionHandler: ^(NSError *error) {
		if (error != nil)
		{
			NSLog(@"scheduleLocalPushRequestWithId create request error: %@",error);
		}
		else
		{
			NSLog(@"scheduleLocalPushRequestWithId create request success");
		}
	}];
}

-(void) registerNotificationCategories: (NSString*) categoryName andActions: (NSArray<PsActionArgIOS*>*) actions
{
	__block UNUserNotificationCenter *center = [UNUserNotificationCenter currentNotificationCenter];
	[center getNotificationCategoriesWithCompletionHandler:^(NSSet<UNNotificationCategory *> *categories){
		NSMutableSet* newCategories = [categories mutableCopy];
		NSMutableArray* actionsArray = [[[NSMutableArray alloc] init] autorelease];

		for (PsActionArgIOS* Action in actions)
		{
			UNNotificationAction* actionObj = [UNNotificationAction actionWithIdentifier: Action.actionId
				title: Action.title options: UNNotificationActionOptionForeground];
			[actionsArray addObject: actionObj];

			NSLog(@"scheduleLocalPushRequestWithId set action %@ for category %@", Action.actionId, categoryName);
		}

		UNNotificationCategory* category = [UNNotificationCategory categoryWithIdentifier: categoryName
			actions: actionsArray
			intentIdentifiers: {}
			options: UNNotificationCategoryOptionCustomDismissAction];

		[newCategories addObject: category];
		[center setNotificationCategories: newCategories];
	}];
}

- (void)userNotificationCenter:(UNUserNotificationCenter *)center
	didReceiveNotificationResponse:(UNNotificationResponse *)response
	withCompletionHandler:(void (^)(void))completionHandler
{
	NSLog(@"didReceiveNotificationResponse doing action push delegate callback %@", response.actionIdentifier);

	NSMutableDictionary* dict = [[[NSMutableDictionary alloc] init] autorelease];
	[dict setObject: response.actionIdentifier forKey: PsNotificationActionFieldName];

	NSDictionary* userInfo = [[[[response notification] request] content] userInfo];
	if (userInfo)
	{
		NSString* activationCode = (NSString*)[userInfo objectForKey: PsNotificationaAtivationCodeFieldName];
		if (activationCode)
		{
			[dict setObject: activationCode forKey: PsNotificationaAtivationCodeFieldName];
		}
	}

	[self saveToTemporaryFile: dict];

	completionHandler();
}

-(void) clearAllLocalNotifications
{
	[[UNUserNotificationCenter currentNotificationCenter] removeAllPendingNotificationRequests];
}

-(void) clearLocalNotificationByIds: (NSArray<NSString*>*) ids
{
	[[UNUserNotificationCenter currentNotificationCenter] removePendingNotificationRequestsWithIdentifiers: ids];
}

-(NSString*) getLastActivationCodeFromDictionary
{
	NSDictionary* dict = [self loadDictionaryFromTemporaryFile];
	if (!dict)
	{
		return nil;
	}
	
	NSString* acticationCodeStr = (NSString*)dict[PsNotificationaAtivationCodeFieldName];
	return acticationCodeStr;
}

-(NSString*) getLastActionIdFromDictionary
{
	NSDictionary* dict = [self loadDictionaryFromTemporaryFile];
	if (!dict)
	{
		return nil;
	}

	NSString* actionIdStr = (NSString*)dict[PsNotificationActionFieldName];
	return actionIdStr;
}

-(void) saveToTemporaryFile: (NSDictionary*) dictionaryData
{
	NSString *tmpDirName = NSTemporaryDirectory();
	NSError *localError = nil;

	[[NSFileManager defaultManager] createDirectoryAtPath:tmpDirName withIntermediateDirectories:TRUE attributes:nil error: &localError];
	if(localError)
	{
		NSLog(@"saveToTemporaryFile Error: %@", localError);
		return;
	}

	NSString *fileURL = [tmpDirName stringByAppendingPathComponent: PsNotificationsDataFileName];
	NSData *data = [NSKeyedArchiver archivedDataWithRootObject: dictionaryData];
	if (!data)
	{
		return;
	}

	[data writeToFile:fileURL atomically:YES];
	NSLog(@"saveToTemporaryFile saved: %@ to %@", dictionaryData, fileURL);
}

-(NSDictionary*) loadDictionaryFromTemporaryFile
{
	NSString *tmpDirName = NSTemporaryDirectory();
	NSString *fileURL = [tmpDirName stringByAppendingPathComponent: PsNotificationsDataFileName];
	NSData *data = [[NSFileManager defaultManager] contentsAtPath: fileURL];
	if (!data)
	{
		NSLog(@"loadDictionaryFromTemporaryFile: failed!");
		return nil;
	}

	NSLog(@"loadDictionaryFromTemporaryFile: %@ from %@", data, fileURL);

	NSDictionary *dictionary = (NSDictionary*) [NSKeyedUnarchiver unarchiveObjectWithData:data];
	return dictionary;
}

-(void) clearNotificationDictionaryData
{
	NSError *localError = nil;
	NSString *tmpDirName = NSTemporaryDirectory();
	NSString *fileURL = [tmpDirName stringByAppendingPathComponent: PsNotificationsDataFileName];
	[[NSFileManager defaultManager] removeItemAtPath: fileURL error: &localError];
}

@end

#endif // PLATFORM_IOS

// Copyright 2018 Mail.Ru Group. All Rights Reserved.

#include "PsPushNotificationsExtendedPrivatePCH.h"
#include "PsPushNotificationsExtended_IOS.h"
#include "Misc/CoreDelegates.h"
#include "PsPushNotificationsExtendedManager.h"

#import <Foundation/Foundation.h>

#if PLATFORM_IOS

@implementation PsActionArgIOS

@synthesize title;
@synthesize actionId;

-(id) initWithTitle: (NSString*) title_ andAction: (NSString*) actionId_
{
	title = title_;
	actionId = actionId_;
	return [super init];
}

@end

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

NSString* PsNotificationsDataFileName = @"local_notification.action";
NSString* PsNotificationActionFieldName = @"Action";

@implementation PsPushNotificationsExtendedDelegate

+(id) sharedInstance
{
	static PsPushNotificationsExtendedDelegate *Instance = nil;
	static dispatch_once_t onceToken;
	dispatch_once(&onceToken, ^{
		Instance = [[self alloc] init];
	});
	return Instance;
}

-(void) requestAuthorization
{
	if (@available(iOS 10, *))
	{
		__block UNUserNotificationCenter *center = [UNUserNotificationCenter currentNotificationCenter];
		center.delegate = self;
		[center requestAuthorizationWithOptions:(UNAuthorizationOptionSound | UNAuthorizationOptionAlert | UNAuthorizationOptionBadge) completionHandler:^(BOOL granted, NSError * _Nullable error){
			if(!error)
			{
				[[UIApplication sharedApplication] registerForRemoteNotifications];

				NSLog(@"RequestAuthorizationWithOptions success");

				int32 types = (int32)(UNAuthorizationOptionSound | UNAuthorizationOptionAlert | UNAuthorizationOptionBadge);
				FFunctionGraphTask::CreateAndDispatchWhenReady([types]()
				{
					FCoreDelegates::ApplicationRegisteredForUserNotificationsDelegate.Broadcast(types);
				}, TStatId(), NULL, ENamedThreads::GameThread);
			}
		}];
	}
}

-(NSString*) scheduleLocalNotificationAtTime: (NSDateComponents*) dateComponents isLocalTime: (bool) bLocal andTitle: (NSString*) title andSubtitle: (NSString*) subtitle andBody: (NSString*) body andAlertAction: (NSString*) action  andCategory: (NSString*) category andImageURL: (NSString*) imageURL andSound: (UNNotificationSoundName*) soundName andBadge: (NSNumber*) badgeNumber
{
	if (@available(iOS 10, *))
	{
		UNUserNotificationCenter *center = [UNUserNotificationCenter currentNotificationCenter];

		UNMutableNotificationContent* Content = [[[UNMutableNotificationContent alloc] init] autorelease];
		Content.title = title;
		Content.subtitle = subtitle;
		Content.body = body;
		if (soundName && [soundName length] > 0)
			Content.sound = [UNNotificationSound soundNamed: soundName];
		else
			Content.sound = UNNotificationSound.defaultSound;
		if (badgeNumber && [badgeNumber intValue] > 0)
			Content.badge = badgeNumber;

		Content.categoryIdentifier = category;

		// load image content
		{
			NSString *attachmentUrlString = imageURL;
			NSURL *url = [NSURL URLWithString:attachmentUrlString];
			if (!url)
			{
				NSLog(@"scheduleLocalNotificationAtTime wrong url");
				return nil;
			}
			NSData *data = [NSData dataWithContentsOfURL:url];
			if (!data)
			{
				NSLog(@"scheduleLocalNotificationAtTime wrong url data");
				return nil;
			}

			NSString *identifierName = @"image.jpg";
			if (url)
			{
				identifierName = [NSString stringWithFormat:@"file.%@",url.lastPathComponent];
			}

			NSString *identifier = identifierName;
			NSString *tmpSubFolderName = [[NSProcessInfo processInfo] globallyUniqueString];

			NSString *fileURLPath = NSTemporaryDirectory();
			NSString *tmpSubFolderURL = [fileURLPath stringByAppendingPathComponent:tmpSubFolderName];
			NSError *localError = nil;
			[[NSFileManager defaultManager] createDirectoryAtPath:tmpSubFolderURL withIntermediateDirectories:TRUE attributes:nil error: &localError];
			if(!localError)
			{
				NSString *fileURL = [tmpSubFolderURL stringByAppendingPathComponent:identifier];
				[data writeToFile:fileURL atomically:YES];
				UNNotificationAttachment *attachment = [UNNotificationAttachment attachmentWithIdentifier:identifier URL:[NSURL fileURLWithPath:fileURL] options:nil error:&localError];
				Content.attachments = [NSArray arrayWithObjects: attachment, nil];
				NSLog(@"scheduleLocalNotificationAtTime attachement assign");
			}
			else
			{
				NSLog(@"scheduleLocalNotificationAtTime localError %@", localError);
			}
		}

		NSCalendar *calendar = [NSCalendar autoupdatingCurrentCalendar];
		UNCalendarNotificationTrigger* trigger = [UNCalendarNotificationTrigger
			triggerWithDateMatchingComponents:dateComponents repeats:NO];

		NSString* PushID = [NSUUID UUID].UUIDString;
		UNNotificationRequest* Request = [UNNotificationRequest requestWithIdentifier: PushID
			content: Content
			trigger: trigger];

		[center addNotificationRequest: Request withCompletionHandler: ^(NSError *error) {
			if (error != nil) {
				NSLog(@"scheduleLocalNotificationAtTime create request error: %@",error);
			}
		}];

		return PushID;
	}

	return nil;
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

-(NSString*) getLastActionId
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

@end

#endif // PLATFORM_IOS

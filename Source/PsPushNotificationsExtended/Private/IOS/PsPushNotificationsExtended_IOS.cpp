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

//				UNNotificationAction* Action = [UNNotificationAction actionWithIdentifier: @"Action1"
//					title: @"Action title" options: UNNotificationActionOptionNone];
//				UNNotificationAction* Dismiss = [UNNotificationAction actionWithIdentifier: @"ActionDismiss"
//					title: @"Action dismiss" options: UNNotificationActionOptionForeground];
//				UNNotificationCategory* Category = [UNNotificationCategory categoryWithIdentifier: @"category1"
//					actions: [NSArray arrayWithObjects: Action, Dismiss, nil]
//					intentIdentifiers: {}
//					options: UNNotificationCategoryOptionCustomDismissAction];
//
//				[center setNotificationCategories: [NSSet setWithObjects: Category, nil]];

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

-(void) scheduleLocalNotificationAtTime: (NSDateComponents*) dateComponents isLocalTime: (bool) bLocal andTitle: (NSString*) title andSubtitle: (NSString*) subtitle andBody: (NSString*) body andAlertAction: (NSString*) action  andCategory: (NSString*) category andImageURL: (NSString*) imageURL
{
	if (@available(iOS 10, *))
	{
		UNUserNotificationCenter *center = [UNUserNotificationCenter currentNotificationCenter];

		UNMutableNotificationContent* Content = [[UNMutableNotificationContent alloc] init];
		Content.title = title;
		Content.subtitle = subtitle;
		Content.body = body;
		Content.sound = UNNotificationSound.defaultSound;
		Content.categoryIdentifier = category;//@"category1";

		// load image content
		{
			NSString *attachmentUrlString = imageURL;//[NSString stringWithFormat:@"%@", @"https://i05.fotocdn.net/s13/112/gallery_s/104/2345428847.jpg"];
			NSURL *url = [NSURL URLWithString:attachmentUrlString];
			if (!url)
			{
				NSLog(@"scheduleLocalNotificationAtTime wrong url");
				return;
			}
			NSData *data = [NSData dataWithContentsOfURL:url];
			if (!data)
			{
				NSLog(@"scheduleLocalNotificationAtTime wrong url data");
				return;
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
	}
}

-(void) registerNotificationCategories: (NSString*) categoryName andActions: (NSArray<PsActionArgIOS*>*) actions
{
	__block UNUserNotificationCenter *center = [UNUserNotificationCenter currentNotificationCenter];
	[center getNotificationCategoriesWithCompletionHandler:^(NSSet<UNNotificationCategory *> *categories){
		NSMutableSet* newCategories = [categories mutableCopy];
		NSMutableArray* actionsArray = [[NSMutableArray alloc] init];

		for (PsActionArgIOS* Action in actions)
		{
			UNNotificationAction* actionObj = [UNNotificationAction actionWithIdentifier: Action.actionId
				title: Action.title options: UNNotificationActionOptionNone];
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
	NSLog(@"didReceiveNotificationResponse performing push action");

	FString ActionId(response.actionIdentifier);
	FString URI;
//	FFunctionGraphTask::CreateAndDispatchWhenReady([ActionId, URI]()
	{
		NSLog(@"didReceiveNotificationResponse doing action push delegate callback");

		UPsPushNotificationsExtendedManager* Manager = UPsPushNotificationsExtendedManager::GetInstance();
		if (Manager && Manager->IsValidLowLevel())
		{
			Manager->OnActionPerform.Broadcast(ActionId, URI);
		}
	}//, TStatId(), NULL, ENamedThreads::GameThread);

	completionHandler();
}

@end

#endif // PLATFORM_IOS

// Copyright 2018 Mail.Ru Group. All Rights Reserved.

#include "PsPushNotificationsExtendedPrivatePCH.h"
#include "PsPushNotificationsExtended_IOS.h"
#include "Misc/CoreDelegates.h"
#include "PsPushNotificationsExtendedManager.h"
#include "IOSAppDelegate+PsPushNotificationsExtended.h"

#import <CommonCrypto/CommonDigest.h>

#if PLATFORM_IOS

@implementation NSString (MyAdditions)
-(NSString*) Ps_GetMD5
{
	const char *cStr = [self UTF8String];
	unsigned char result[CC_MD5_DIGEST_LENGTH];
	CC_MD5( cStr, (int)strlen(cStr), result ); // This is the md5 call
	return [NSString stringWithFormat:
			@"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
			result[0], result[1], result[2], result[3],
			result[4], result[5], result[6], result[7],
			result[8], result[9], result[10], result[11],
			result[12], result[13], result[14], result[15]
			];
}
@end

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

@synthesize currentActivationCode;
@synthesize currentActionId;

+(id) sharedInstance
{
	static PsPushNotificationsExtendedDelegate *Instance = nil;
	static dispatch_once_t onceToken;
	dispatch_once(&onceToken, ^{
		Instance = [[self alloc] init];
		NSLog(@"%s instance created", __PRETTY_FUNCTION__);
	});
	return Instance;
}

-(id) init
{
	self = [super init];
	if (self)
	{
		NSLog(@"%s init delegate", __PRETTY_FUNCTION__);
		UNUserNotificationCenter *center = [UNUserNotificationCenter currentNotificationCenter];
		center.delegate = self;
	}

	return self;
}

-(void) requestAuthorization
{
	// Get prev notifications info first
	self.currentActivationCode = [self getLastActivationCodeFromDictionary];
	self.currentActionId = [self getLastActionIdFromDictionary];
	[self clearNotificationDictionaryData];

	// Request for notifications
	if (@available(iOS 10, *))
	{
		NSLog(@"%s request authorization", __PRETTY_FUNCTION__);

		__block UNUserNotificationCenter *center = [UNUserNotificationCenter currentNotificationCenter];
		[center requestAuthorizationWithOptions:(UNAuthorizationOptionSound | UNAuthorizationOptionAlert | UNAuthorizationOptionBadge) completionHandler:^(BOOL granted, NSError * _Nullable error){
			if(!error)
			{
				[[UIApplication sharedApplication] registerForRemoteNotifications];

				NSLog(@"%s requestAuthorizationWithOptions success", __PRETTY_FUNCTION__);

				int32 types = (int32)(UNAuthorizationOptionSound | UNAuthorizationOptionAlert | UNAuthorizationOptionBadge);
				FFunctionGraphTask::CreateAndDispatchWhenReady([types]()
				{
					FCoreDelegates::ApplicationRegisteredForUserNotificationsDelegate.Broadcast(types);
				}, TStatId(), NULL, ENamedThreads::GameThread);
			}
		}];
	}
}

-(UNNotificationAttachment*) getAttachementWithId:(NSString*) attachementId andURL:(NSString*) fileURL
{
	NSURL *url = [NSURL URLWithString: fileURL];
	if (!url)
	{
		return nil;
	}
	NSArray* paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
	NSString* cacheURLPath = [paths objectAtIndex : 0];

	NSError* localError = nil;
	NSString* tmpPath = [NSString stringWithFormat:@"%@/%@.%@", cacheURLPath, attachementId, url.pathExtension];
	[[NSFileManager defaultManager] copyItemAtPath: fileURL toPath: tmpPath error: &localError];
	if (localError)
	{
		NSLog(@"%s attachement create error %@", __PRETTY_FUNCTION__, localError);
		return nil;
	}

	UNNotificationAttachment *attachment = [UNNotificationAttachment attachmentWithIdentifier: attachementId URL: [NSURL fileURLWithPath: tmpPath] options:nil error: &localError];
	if (localError)
	{
		NSLog(@"%s attachement create error %@", __PRETTY_FUNCTION__, localError);
		return nil;
	}

	return attachment;
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
		NSString *identifier = @"";
		NSURL *url = [NSURL URLWithString:imageURL];
		if (url)
		{
			identifier = [NSString stringWithFormat:@"%@.%@", [imageURL Ps_GetMD5], url.pathExtension];
		}
		NSString* attachementId = [NSString stringWithFormat:@"%@.%@", PushID, identifier];

		if (bIsLocalResource || !url)
		{
			UNMutableNotificationContent* Content = [self notificationContentWith: title andSubtitle: subtitle andBody: body andSound:soundName andBadge:  badgeNumber andActivationCode: activationCode andCategory: category];

			if (url)
			{
				NSError *localError = nil;
				UNNotificationAttachment *attachment = [self getAttachementWithId: attachementId andURL: imageURL];
				if(attachment)
				{
					Content.attachments = [NSArray arrayWithObjects: attachment, nil];
					NSLog(@"%s local attachement assign", __PRETTY_FUNCTION__);
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
				NSLog(@"%s creating notifications cache directory: %@", __PRETTY_FUNCTION__, fileURLPath);
				NSError *createCacheDirError = nil;
				[[NSFileManager defaultManager] createDirectoryAtPath: fileURLPath withIntermediateDirectories: YES attributes: nil error: &createCacheDirError];

				if (createCacheDirError)
				{
					NSLog(@"%s %@", __PRETTY_FUNCTION__, createCacheDirError);
					UNMutableNotificationContent* Content = [self notificationContentWith: title andSubtitle: subtitle andBody: body andSound:soundName andBadge: badgeNumber andActivationCode: activationCode andCategory: category];
					[self scheduleLocalPushRequestWithId: PushID atTime: dateComponents andContent: Content];
					return PushID;
				}
			}

			NSLog(@"%s notifications cache directory at: %@", __PRETTY_FUNCTION__, fileURLPath);
			NSString *fileURL = [fileURLPath stringByAppendingPathComponent: identifier];

			if ([[NSFileManager defaultManager] fileExistsAtPath: fileURL])
			{
				NSLog(@"%s cache file exists %@", __PRETTY_FUNCTION__, fileURL);

				NSError *localError = nil;
				UNMutableNotificationContent* Content = [self notificationContentWith: title andSubtitle: subtitle andBody: body andSound:soundName andBadge: badgeNumber andActivationCode: activationCode andCategory: category];

				UNNotificationAttachment *attachment = [self getAttachementWithId: attachementId andURL: fileURL];
				if (attachment)
				{
					NSLog(@"%s assigning attachement", __PRETTY_FUNCTION__);
					Content.attachments = [NSArray arrayWithObjects: attachment, nil];
				}

				[self scheduleLocalPushRequestWithId: PushID atTime: dateComponents andContent: Content];
				return PushID;
			}

			NSLog(@"%s remote content download", __PRETTY_FUNCTION__);
			NSURLSessionDownloadTask *task = [[NSURLSession sharedSession] downloadTaskWithURL: url
				completionHandler:^(NSURL *location, NSURLResponse *response, NSError *error) {
				@synchronized(self)
				{
					UNMutableNotificationContent* AsyncContent = [self notificationContentWith: title andSubtitle: subtitle andBody: body andSound:soundName andBadge:  badgeNumber andActivationCode: activationCode andCategory: category];

					NSLog(@"%s downloadTaskWithURL %@", __PRETTY_FUNCTION__, location);
					if (error)
					{
						NSLog(@"%s error %@", __PRETTY_FUNCTION__, error);
						[self scheduleLocalPushRequestWithId: PushID atTime: dateComponents andContent: AsyncContent];
						return;
					}

					if (!location)
					{
						NSLog(@"%s location is nil", __PRETTY_FUNCTION__);
						[self scheduleLocalPushRequestWithId: PushID atTime: dateComponents andContent: AsyncContent];
						return;
					}

					NSError *localError = nil;

					if (![[NSFileManager defaultManager] fileExistsAtPath: fileURL])
					{
						NSLog(@"%s moving tmp file: %@ to path: %@", __PRETTY_FUNCTION__, [location path], fileURL);
						[[NSFileManager defaultManager] moveItemAtPath: [location path] toPath: fileURL error: &localError];
						if(localError)
						{
							NSLog(@"%s can not move file: %@", __PRETTY_FUNCTION__, localError);
							[self scheduleLocalPushRequestWithId: PushID atTime: dateComponents andContent: AsyncContent];
							return;
						}
					}

					// Creating attachement
					NSLog(@"%s setting up notification attachement", __PRETTY_FUNCTION__);
					UNNotificationAttachment *attachment = [self getAttachementWithId: attachementId andURL: fileURL];
					if (!attachment)
					{
						NSLog(@"%s remote attachement assign error %@", __PRETTY_FUNCTION__, localError);
						[self scheduleLocalPushRequestWithId: PushID atTime: dateComponents andContent: AsyncContent];
						return;
					}

					NSLog(@"%s assigning attachement as notification content", __PRETTY_FUNCTION__);
					AsyncContent.attachments = [NSArray arrayWithObjects: attachment, nil];
					[self scheduleLocalPushRequestWithId: PushID atTime: dateComponents andContent: AsyncContent];

					NSLog(@"%s remote attachement assign success", __PRETTY_FUNCTION__);
				}
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
			NSLog(@"%s create request error: %@", __PRETTY_FUNCTION__, error);
		}
		else
		{
			NSLog(@"%s create request success", __PRETTY_FUNCTION__);
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

			NSLog(@"%s set action %@ for category %@", __PRETTY_FUNCTION__, Action.actionId, categoryName);
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
	NSLog(@"%s doing action push delegate callback %@", __PRETTY_FUNCTION__, response.actionIdentifier);

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
		NSLog(@"%s Error: %@", __PRETTY_FUNCTION__, localError);
		return;
	}

	NSString *fileURL = [tmpDirName stringByAppendingPathComponent: PsNotificationsDataFileName];
	NSData *data = [NSKeyedArchiver archivedDataWithRootObject: dictionaryData];
	if (!data)
	{
		return;
	}

	[data writeToFile:fileURL atomically:YES];
	NSLog(@"%s saved: %@ to %@", __PRETTY_FUNCTION__, dictionaryData, fileURL);
}

-(NSDictionary*) loadDictionaryFromTemporaryFile
{
	NSString *tmpDirName = NSTemporaryDirectory();
	NSString *fileURL = [tmpDirName stringByAppendingPathComponent: PsNotificationsDataFileName];
	NSData *data = [[NSFileManager defaultManager] contentsAtPath: fileURL];
	if (!data)
	{
		NSLog(@"%s: failed!", __PRETTY_FUNCTION__);
		return nil;
	}

	NSLog(@"%s: %@ from %@", __PRETTY_FUNCTION__, data, fileURL);

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

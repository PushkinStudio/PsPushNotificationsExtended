// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.

package com.epicgames.ue4;

import android.util.Log;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.support.v4.app.NotificationCompat;

public class PsLocalNotificationsExtendedReceiver extends BroadcastReceiver
{
	public void onReceive(Context context, Intent intent)
	{
		int notificationID = intent.getIntExtra("local-notification-ID" , 0);
		String title  = intent.getStringExtra("local-notification-title");
		String details  = intent.getStringExtra("local-notification-body");
		String action = intent.getStringExtra("local-notification-action");
		String activationEvent = intent.getStringExtra("local-notification-activationEvent");

		if(title == null || details == null || action == null || activationEvent == null)
		{
			// Do not schedule any local notification if any allocation failed
			return;
		}

		// Open UE4 app if clicked
		Intent notificationIntent = new Intent(context, GameActivity.class);

		// launch if closed but resume if running
		notificationIntent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_SINGLE_TOP);
		notificationIntent.putExtra("localNotificationID" , notificationID);
		notificationIntent.putExtra("localNotificationAppLaunched" , true);
		notificationIntent.putExtra("localNotificationLaunchActivationEvent", activationEvent);

		int notificationIconID = context.getResources().getIdentifier("ic_notification", "drawable", context.getPackageName());
		if (notificationIconID == 0)
		{
			notificationIconID = context.getResources().getIdentifier("icon", "drawable", context.getPackageName());
		}
		PendingIntent pendingNotificationIntent = PendingIntent.getActivity(context, notificationID, notificationIntent, 0);

		NotificationCompat.Builder builder = new NotificationCompat.Builder(context)
			.setSmallIcon(notificationIconID)
			.setContentIntent(pendingNotificationIntent)
			.setWhen(System.currentTimeMillis())
			.setTicker(details)		// note: will not show up on Lollipop up except for accessibility
			.setContentTitle(title)
			.setContentText(details)
			.setVisibility(NotificationCompat.VISIBILITY_PUBLIC);

		// Checking action buttons
		for (int i = 0;;i++)
		{
			String actionTitleExtraTag = "local-notification-cateroty-action-" + i + "-title";
			String actionIdExtraTag = "local-notification-cateroty-action-" + i + "-id";
			String actionTitle = intent.getStringExtra(actionTitleExtraTag);
			String actionId = intent.getStringExtra(actionIdExtraTag);
			if (actionTitle == null || actionId == null || actionTitle.length() == 0 || actionId.length() == 0)
			{
				break;
			}

			Intent notificationActionIntent = new Intent(context, PsLocalNotificationsExtendedActionReceiver.class);
//			notificationActionIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_CLEAR_TASK);
			notificationActionIntent.putExtra("local-notification-ID", notificationID);
			notificationActionIntent.putExtra("local-notification-action", action);
			notificationActionIntent.putExtra("local-notification-activationEvent", activationEvent);
			notificationActionIntent.putExtra("local-notification-activationAction", actionId);
			notificationActionIntent.setAction("com.my.PsLocalNotificationsExtendedActionReceiver.Broadcast");

//			PendingIntent actionPendingIntent = PendingIntent.getService(context, i, notificationActionIntent, PendingIntent.FLAG_UPDATE_CURRENT);
			PendingIntent actionPendingIntent = PendingIntent.getBroadcast(context, 0, notificationActionIntent, 0);
			builder.addAction(android.R.drawable.star_on, actionTitle, pendingNotificationIntent);
		}

		if (android.os.Build.VERSION.SDK_INT >= 26)
		{
			builder.setChannelId(GameActivity.PS_PUSH_NOTIFICATIONS_CHANNEL_ID);
		}

		if (android.os.Build.VERSION.SDK_INT >= 21)
		{
			builder.setColor(0xff0e1e43);
		}
		Notification notification = builder.build();

		// Stick with the defaults
		notification.flags |= Notification.FLAG_AUTO_CANCEL;
		notification.defaults |= Notification.DEFAULT_SOUND | Notification.DEFAULT_VIBRATE;

		NotificationManager notificationManager = (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);

		// show the notification
		notificationManager.notify(notificationID, notification); 
	}
};

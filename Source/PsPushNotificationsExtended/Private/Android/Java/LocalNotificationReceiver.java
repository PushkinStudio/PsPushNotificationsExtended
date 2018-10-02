// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.

 package com.epicgames.ue4;
 
 import android.app.Notification;
 import android.app.NotificationManager;
 import android.app.PendingIntent;
 import android.content.BroadcastReceiver;
 import android.content.Context;
 import android.content.Intent;
 import android.support.v4.app.NotificationCompat;
 
 public class LocalNotificationReceiver extends BroadcastReceiver
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
			.setContentText(details)								// @pushkin AWM-17179
			.setVisibility(NotificationCompat.VISIBILITY_PUBLIC);	// @pushkin AWM-17171

		// @pushkin AWM-17193 BEGIN <-----
		if (android.os.Build.VERSION.SDK_INT >= 26)
		{
			builder.setChannelId(GameActivity.UE4_DEFAULT_CHANNEL_ID);
		}
		// @pushkin AWM-17193 END <-----

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
 }
 
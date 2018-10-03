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

public class PsLocalNotificationsExtendedActionReceiver extends BroadcastReceiver
{
	public void onReceive(Context context, Intent intent)
	{
		int notificationID = intent.getIntExtra("local-notification-ID" , 0);
		String activationEvent = intent.getStringExtra("local-notification-activationEvent");
		String activationActionEvent = intent.getStringExtra("local-notification-activationActionEvent");

		Intent notificationIntent = new Intent(context, GameActivity.class);
		notificationIntent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_SINGLE_TOP);
		notificationIntent.putExtra("localNotificationID" , notificationID);
		notificationIntent.putExtra("localNotificationAppLaunched" , true);
		notificationIntent.putExtra("localNotificationLaunchActivationEvent", activationEvent);
		notificationIntent.putExtra("localNotificationLaunchActivationAction", activationActionEvent);

		PendingIntent pendingNotificationIntent = PendingIntent.getActivity(context, notificationID, notificationIntent, 0);

		NotificationCompat.Builder builder = new NotificationCompat.Builder(context)
			.setContentIntent(pendingNotificationIntent)
			.setChannelId(GameActivity.PS_PUSH_NOTIFICATIONS_CHANNEL_ID);

		Notification notification = builder.build();

		NotificationManager notificationManager = (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);
		notificationManager.notify(notificationID, notification); 

//		NotificationManagerCompat.from(context).cancel(notificationID, 0);
	}
};

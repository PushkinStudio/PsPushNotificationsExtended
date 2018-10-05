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
import android.util.Log;

public class PsLocalNotificationsExtendedActionReceiver extends BroadcastReceiver
{
	public void onReceive(Context context, Intent intent)
	{
		Intent it = new Intent(Intent.ACTION_CLOSE_SYSTEM_DIALOGS);
		context.sendBroadcast(it);

		NotificationManager notificationManager = (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);

		int notificationID = intent.getIntExtra("local-notification-ID" , 0);


		String activationEvent = intent.getStringExtra("local-notification-activationEvent");
		String activationActionEvent = intent.getStringExtra("local-notification-activationAction");

		Intent notificationIntent = new Intent(context, GameActivity.class);
		notificationIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_CLEAR_TASK);

		Log.d("onReceive", "NOID EXT: " + notificationID + " " + activationActionEvent);
		notificationIntent.putExtra("localNotificationAppLaunched", true);
		notificationIntent.putExtra("localNotificationLaunchActivationAction", activationActionEvent);
		notificationIntent.putExtra("localNotificationLaunchActivationEvent", activationEvent);

		context.startActivity(notificationIntent);
	}
};

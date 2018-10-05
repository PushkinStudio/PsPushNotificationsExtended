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
import java.io.InputStream;
import java.net.URL;
import java.net.HttpURLConnection;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.AsyncTask;

public class PsLocalNotificationsExtendedReceiver extends BroadcastReceiver
{
	public void onReceive(Context context, Intent intent)
	{
		int notificationID = intent.getIntExtra("local-notification-ID" , 0);
		String title  = intent.getStringExtra("local-notification-title");
		String details  = intent.getStringExtra("local-notification-body");
		String action = intent.getStringExtra("local-notification-action");
		String activationEvent = intent.getStringExtra("local-notification-activationEvent");
		String contentURL = intent.getStringExtra("local-notification-contentURL");

		Log.d("onReceive", "NOID: " + notificationID);

		if(title == null || details == null || action == null || activationEvent == null)
		{
			// Do not schedule any local notification if any allocation failed
			return;
		}

		// Checking action buttons
		String buttonsContext = "";
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

			buttonsContext += actionTitle + "=" + actionId + "|";
		}

		new sendNotification(context).execute(String.valueOf(notificationID), title, details, action, activationEvent, contentURL, buttonsContext);
	}

	private class sendNotification extends AsyncTask<String, Void, Bitmap>
	{
		Context context;
		int notificationID;
		String title;
		String details;
		String action;
		String activationEvent;
		String contentURL;
		String buttonsContext;

		public sendNotification(Context context)
		{
			super();
			this.context = context;
		}

		@Override
		protected Bitmap doInBackground(String... params)
		{
			InputStream in;

			notificationID = Integer.parseInt(params[0]);
			title = params[1];
			details = params[2];
			action = params[3];
			activationEvent = params[4];
			contentURL = params[5];
			buttonsContext = params[6];

			if (contentURL != null && contentURL.length() > 0)
			{
				try
				{
					in = new URL(contentURL).openStream();
					Bitmap bmp = BitmapFactory.decodeStream(in);
					return bmp;
				}
				catch (Exception e)
				{
					e.printStackTrace();
				}
			}

			return null;
		}

		@Override
		protected void onPostExecute(Bitmap result)
		{
			super.onPostExecute(result);

//		Intent notificationIntent = new Intent(context, GameActivity.class);
			Intent notificationIntent = new Intent(context, PsLocalNotificationsExtendedActionReceiver.class);
//			notificationIntent.putExtra("local-notification-ID", notificationID);
//			notificationIntent.putExtra("local-notification-action", action);
//			notificationIntent.putExtra("local-notification-activationEvent", activationEvent);
			notificationIntent.setAction("com.my.PsLocalNotificationsExtendedActionReceiver.Broadcast");

			Log.d("onReceive", "NOID TASK: " + notificationID);
		// launch if closed but resume if running
			notificationIntent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_SINGLE_TOP);
//		notificationIntent.putExtra("localNotificationID" , notificationID);
//		notificationIntent.putExtra("localNotificationAppLaunched" , true);
//		notificationIntent.putExtra("localNotificationLaunchActivationEvent", activationEvent);

			int notificationIconID = context.getResources().getIdentifier("ic_notification", "drawable", context.getPackageName());
			if (notificationIconID == 0)
			{
				notificationIconID = context.getResources().getIdentifier("icon", "drawable", context.getPackageName());
			}
//		PendingIntent pendingNotificationIntent = PendingIntent.getActivity(context, notificationID, notificationIntent, PendingIntent.FLAG_UPDATE_CURRENT);
			PendingIntent pendingNotificationIntent = PendingIntent.getBroadcast(context, notificationID, notificationIntent, PendingIntent.FLAG_UPDATE_CURRENT);

			NotificationCompat.Builder builder = new NotificationCompat.Builder(context)
				.setSmallIcon(notificationIconID)
				.setContentIntent(pendingNotificationIntent)
				.setWhen(System.currentTimeMillis())
				.setTicker(details)		// note: will not show up on Lollipop up except for accessibility
				.setContentTitle(title)
				.setContentText(details)
				.setVisibility(NotificationCompat.VISIBILITY_PUBLIC);

			// Setting up image
			if (result != null)
			{
				builder.setStyle(new NotificationCompat.BigPictureStyle().bigPicture(result));
				Log.d("onReceive", "GOT URL: " + contentURL);
			}

			// Checking action buttons
			if (buttonsContext.length() > 0)
			{
				Log.d("onReceive", "GOT_buttons: " + buttonsContext);
				String[] buttons = buttonsContext.split("[|]");
				for (int i = 0; i < buttons.length; i++)
				{
					String button = buttons[i];
					String[] parts = button.split("=");
					Log.d("onReceive", "GOT_buttons_parts: " + button);
					if (parts.length > 1)
					{
						String actionTitle = parts[0];
						String actionId = parts[1];
						Log.d("onReceive", "GOT_buttons_title+id: " + actionTitle + " " + actionId);

						if (actionTitle == null || actionId == null || actionTitle.length() == 0 || actionId.length() == 0)
						{
							break;
						}

						Intent notificationActionIntent = new Intent(context, PsLocalNotificationsExtendedActionReceiver.class);
						notificationActionIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_CLEAR_TASK);
						int actionNotificationID = notificationID;
						notificationActionIntent.putExtra("local-notification-ID", actionNotificationID);
						notificationActionIntent.putExtra("local-notification-action", action);
						notificationActionIntent.putExtra("local-notification-activationEvent", activationEvent);
						notificationActionIntent.putExtra("local-notification-activationAction", actionId);
						notificationActionIntent.setAction("com.my.PsLocalNotificationsExtendedActionReceiver.Broadcast");

//			PendingIntent actionPendingIntent = PendingIntent.getService(context, i, notificationActionIntent, PendingIntent.FLAG_UPDATE_CURRENT);
						PendingIntent actionPendingIntent = PendingIntent.getBroadcast(context, actionNotificationID, notificationActionIntent, PendingIntent.FLAG_UPDATE_CURRENT);
						builder.addAction(android.R.drawable.star_on, actionTitle, pendingNotificationIntent);
					}
				}
			}

			builder.setChannelId(GameActivity.PS_PUSH_NOTIFICATIONS_CHANNEL_ID);
			builder.setColor(0xff0e1e43);

			Notification notification = builder.build();

			// Stick with the defaults
			notification.flags |= Notification.FLAG_AUTO_CANCEL;
			notification.defaults |= Notification.DEFAULT_SOUND | Notification.DEFAULT_VIBRATE;

			NotificationManager notificationManager = (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);

			// show the notification
			notificationManager.notify(notificationID, notification);
		}
    };
};
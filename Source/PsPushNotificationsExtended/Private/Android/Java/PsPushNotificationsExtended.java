import android.app.NotificationManager;
import android.app.NotificationChannel;


	public void onCreate(Bundle savedInstanceState)
	{
		if (android.os.Build.VERSION.SDK_INT >= 26)
		{
			NotificationManager NotifManager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);

			String name = "Channel";
			String description = "Desc";
			int importance = NotificationManager.IMPORTANCE_LOW;

			NotificationChannel NotifChannel = new NotificationChannel(UE4_DEFAULT_CHANNEL_ID, name, importance);
			NotifChannel.setDescription(description);

			NotifManager.createNotificationChannel(NotifChannel);
		}
	}

	public void AndroidThunkJava_LocalNotificationScheduleAtTime(String targetDateTime, boolean localTime, String title, String body, String action, String activationEvent) 
	{
		int notificationID = LocalNotificationGetID();

		// Create callback for PendingIntent
		Intent notificationIntent = new Intent(this, LocalNotificationReceiver.class); 

		// Add user-provided data
		notificationIntent.putExtra("local-notification-ID", notificationID);
		notificationIntent.putExtra("local-notification-title", title);
		notificationIntent.putExtra("local-notification-body", body);
		notificationIntent.putExtra("local-notification-action", action);
		notificationIntent.putExtra("local-notification-activationEvent", activationEvent);
		
		// Designate the callback as a PendingIntent
		PendingIntent pendingIntent = PendingIntent.getBroadcast(this, notificationID, notificationIntent, PendingIntent.FLAG_UPDATE_CURRENT);

		TimeZone targetTimeZone = TimeZone.getTimeZone("UTC");

		if(localTime) 
		{
			targetTimeZone = TimeZone.getDefault();
		}

		DateFormat targetDateFormat = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
		targetDateFormat.setTimeZone(targetTimeZone);

		Date targetDate = new Date();

		try 
		{
			targetDate = targetDateFormat.parse(targetDateTime);
		} 

		catch (ParseException e) 
		{
			e.printStackTrace();
			return;
		}

		Date currentDate = new Date();

		long msDiff = targetDate.getTime() - currentDate.getTime();

		if(msDiff < 0)
		{
			return;
		}

		long futureTimeInMillis = SystemClock.elapsedRealtime() + msDiff;//Calculate the time to run the callback
		AlarmManager alarmManager = (AlarmManager)getSystemService(Context.ALARM_SERVICE);

		//Schedule the operation by using AlarmService
		alarmManager.set(AlarmManager.ELAPSED_REALTIME_WAKEUP, futureTimeInMillis, pendingIntent);
	}

	public void AndroidThunkJava_LocalNotificationClearAll()
	{
		ArrayList<Integer> idList = LocalNotificationGetIDList(); 

		for(int curID : idList)
		{
			AlarmManager alarmManager = (AlarmManager)getSystemService(Context.ALARM_SERVICE);
			PendingIntent pendingIntent = PendingIntent.getBroadcast(this, curID, new Intent(this, LocalNotificationReceiver.class), PendingIntent.FLAG_UPDATE_CURRENT);
			pendingIntent.cancel();
			alarmManager.cancel(pendingIntent);
		}
	}

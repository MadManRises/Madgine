package com.Madgine;

import android.app.Activity;

import com.google.android.gms.games.*; 
import com.google.android.gms.tasks.*; 

class VoidReceiver implements OnCompleteListener{
	long ptr;

	private native static void setValue(long ptr);
	private native static void setError(long ptr, Exception ex);

	public VoidReceiver(long ptr){
		this.ptr = ptr;
	}

	public void onComplete(Task task){
		if (task.isSuccessful()){
			setValue(ptr);
		}else{
			setError(ptr, task.getException());
		}
	}
}

public class PlayServices {

	private static Activity activity;
	private static AchievementsClient achievements;

	public static void Setup(Activity _activity)
	{
		activity = _activity;
		achievements = PlayGames.getAchievementsClient(activity);
	}

	public static void Achievements_Unlock(String name, long receiver) 
	{
		String fullName = "achievement_" + name.replaceAll("\\(","").replaceAll("\\)","").replaceAll(" ", "_").toLowerCase();
		String id = getStringResourceByName(fullName);
		Task<Void> task = achievements.unlockImmediate(id);
		task.addOnCompleteListener(new VoidReceiver(receiver));
	}
	
	private static String getStringResourceByName(String aString) {
      String packageName = activity.getPackageName();
      int resId = activity.getResources().getIdentifier(aString, "string", packageName);
      return activity.getString(resId);
    }
}
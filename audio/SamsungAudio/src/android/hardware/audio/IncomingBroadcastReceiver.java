package android.hardware.audio;

import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.telephony.TelephonyManager;
import android.util.Log;

/**
 * Created by DELL on 2017/9/11.
 */
public class PhoneStateChangeReceive extends BroadcastReceiver {

    public final static String CALL_STATE_RINGING="CALL_STATE_RINGING";
    public final static String CALL_STATE_OFFHOOK="CALL_STATE_OFFHOOK";
    public final static String CALL_STATE_IDLE="CALL_STATE_IDLE";
    private final static String TAG=PhoneStateChangeReceive.class.getSimpleName();
    @Override
    public void onReceive(Context context, Intent intent) {
        Intent phoneIntent=new Intent();
        TelephonyManager tManager = (TelephonyManager) context
                .getSystemService(Service.TELEPHONY_SERVICE);
        switch (tManager.getCallState()) {
            case TelephonyManager.CALL_STATE_RINGING:
                Log.d(TAG, "onReceive: CALL_STATE_RINGING");
                phoneIntent.setAction(CALL_STATE_RINGING);
                SystemProperties.set("vendor.audio.a2dp.pickup", "false");
                break;
            case TelephonyManager.CALL_STATE_OFFHOOK:
                Log.d(TAG, "onReceive: CALL_STATE_OFFHOOK");
                phoneIntent.setAction(CALL_STATE_OFFHOOK);
                SystemProperties.set("vendor.audio.a2dp.pickup", "true");
                break;
            case TelephonyManager.CALL_STATE_IDLE:
                Log.d(TAG, "onReceive: CALL_STATE_IDLE ");
                phoneIntent.setAction(CALL_STATE_IDLE);
                SystemProperties.set("vendor.audio.a2dp.pickup", "false");
                break;
            }
            context.sendBroadcast(phoneIntent);
    }
}
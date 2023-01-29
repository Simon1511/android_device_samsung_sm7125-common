/*
 * Copyright (c) 2023 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package android.hardware.audio;

import android.app.Service;
import android.content.Intent;
import android.os.SystemProperties;
import android.os.IBinder;
import android.util.Log;
import android.os.Handler;
import android.media.AudioManager;
import android.media.AudioDeviceInfo;

public class SamsungAudioService extends Service {
    private static final String TAG = "SamsungAudioService";
    private static final boolean DEBUG = true;

    private Handler mHandler;
    private static Runnable mRunnable;
    private AudioManager mAudioManager;

    @Override
    public void onCreate() {
        if (DEBUG) Log.d(TAG, "SamsungAudioService Started");

        mAudioManager = getSystemService(AudioManager.class);


        mHandler = new Handler();
        mRunnable = new Runnable() {
            public void run() {
                if (DEBUG) Log.d(TAG, "onCreate: " + TAG + " is running");

                if (mAudioManager.getMode() == AudioManager.MODE_IN_CALL &&
                        mAudioManager.getCommunicationDevice().getType() == AudioDeviceInfo.TYPE_BLUETOOTH_A2DP) {
                    Log.e(TAG, "A2DP");
                    SystemProperties.set("vendor.audio.a2dp.call", "true");
                }

                if (mAudioManager.getMode() == AudioManager.MODE_IN_COMMUNICATION) {

                    if (mAudioManager.getCommunicationDevice().getType() == AudioDeviceInfo.TYPE_BUILTIN_EARPIECE) {
                        Log.e(TAG, "Earpiece");
                        SystemProperties.set("vendor.audio.earpiece.voip", "true");
                        SystemProperties.set("vendor.audio.speaker.voip", "false");
                    }

                    if (mAudioManager.getCommunicationDevice().getType() == AudioDeviceInfo.TYPE_BUILTIN_SPEAKER
                            || mAudioManager.getCommunicationDevice().getType() == AudioDeviceInfo.TYPE_BUILTIN_SPEAKER_SAFE) {
                        Log.e(TAG, "Speaker");
                        SystemProperties.set("vendor.audio.speaker.voip", "true");
                        SystemProperties.set("vendor.audio.earpiece.voip", "false");

                    }
                }
                mHandler.postDelayed(mRunnable, 100);
            }
        };

        mHandler.postDelayed(mRunnable, 0);
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        if (DEBUG) Log.d(TAG, "Starting service");
        return START_STICKY;
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }
}

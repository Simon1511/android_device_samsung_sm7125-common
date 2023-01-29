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

package android.hardware.media;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothA2dp;
import android.media.AudioManager;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;
import android.os.SystemProperties;

public class DeviceChangedReceiver extends BroadcastReceiver {

    private static final String TAG = "SamsungAudio";

    @Override
    public void onReceive(Context context, Intent intent) {
        final String action = intent.getAction();

        if (action.equals(BluetoothA2dp.ACTION_CONNECTION_STATE_CHANGED)) {
            Log.e(TAG, "ACTION_CONNECTION_STATE_CHANGED");
            final int state = intent.getIntExtra(BluetoothA2dp.EXTRA_STATE, BluetoothAdapter.ERROR);
            switch(state) {
                case BluetoothA2dp.STATE_CONNECTED:
                    Log.e(TAG, "STATE_CONNECTED");
                    SystemProperties.set("vendor.audio.a2dp.call", "true");
                    break;
                case BluetoothA2dp.STATE_CONNECTING:
                    Log.e(TAG, "STATE_CONNECTING");
                    break;
                case BluetoothA2dp.STATE_DISCONNECTED:
                    Log.e(TAG, "STATE_DISCONNECTED");
                    SystemProperties.set("vendor.audio.a2dp.call", "false");
                    break;
                case BluetoothA2dp.STATE_DISCONNECTING:
                    Log.e(TAG, "STATE_DISCONNECTING");
                    break;
            }
        }
    }
}

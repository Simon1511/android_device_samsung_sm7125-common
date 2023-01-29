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
import android.content.BroadcastReceiver;
import android.os.SystemProperties;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

public class BluetoothStateReceiver extends BroadcastReceiver {

    private static final String TAG = "SamsungAudioBTStateReceiver";
    private static final boolean DEBUG = false;

    @Override
    public void onReceive(Context context, Intent intent) {
        final String action = intent.getAction();

        if (action.equals(BluetoothAdapter.ACTION_CONNECTION_STATE_CHANGED)) {
            final int state = intent.getIntExtra(BluetoothAdapter.EXTRA_STATE, BluetoothAdapter.ERROR);

            switch (state) {
                case BluetoothAdapter.STATE_CONNECTED:
                    if (DEBUG) Log.d(TAG, "STATE_CONNECTED");
                    SystemProperties.set("vendor.audio.a2dp.connected", "true");
                    break;
                case BluetoothAdapter.STATE_CONNECTING:
                    if (DEBUG) Log.d(TAG, "STATE_CONNECTING");
                    break;
                case BluetoothAdapter.STATE_DISCONNECTED:
                    if (DEBUG) Log.d(TAG, "STATE_DISCONNECTED");
                    SystemProperties.set("vendor.audio.a2dp.connected", "false");
                    break;
                case BluetoothAdapter.STATE_DISCONNECTING:
                    if (DEBUG) Log.d(TAG, "STATE_DISCONNECTING");
                    break;
            }
        }
    }
}

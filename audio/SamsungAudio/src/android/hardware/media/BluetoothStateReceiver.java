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

import android.bluetooth.BluetoothHeadset;
import android.content.BroadcastReceiver;
import android.os.SystemProperties;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

public class BluetoothStateReceiver extends BroadcastReceiver {

    private static final String TAG = "SamsungBluetoothStateReceiver";
    private static final boolean DEBUG = false;

    @Override
    public void onReceive(Context context, Intent intent) {
        final String action = intent.getAction();
            final int state = intent.getIntExtra(BluetoothHeadset.EXTRA_STATE, BluetoothHeadset.STATE_AUDIO_DISCONNECTED);

        switch (state) {
            case BluetoothHeadset.STATE_CONNECTED:
                if (DEBUG) Log.d(TAG, "STATE_CONNECTED");
                SystemProperties.set("vendor.audio.a2dp.connected", "true");
                break;
            case BluetoothHeadset.STATE_DISCONNECTED:
                if (DEBUG) Log.d(TAG, "STATE_DISCONNECTED");
                SystemProperties.set("vendor.audio.a2dp.connected", "false");
                break;
        }
    }
}

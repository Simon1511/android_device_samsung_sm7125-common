/*
 * Copyright (C) 2022 The LineageOS Project
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

package android.hardware.display;

import android.content.Context;
import android.content.Intent;
import android.os.UserHandle;
import android.util.Log;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.List;

public final class Utils {

    private static final String TAG = "UDFPSUtils";
    private static final boolean DEBUG = false;

    protected static void startService(Context context) {
        if (DEBUG) Log.d(TAG, "Starting service");
        context.startServiceAsUser(new Intent(context, SamsungUDFPSService.class),
                UserHandle.CURRENT);
    }

    protected static void stopService(Context context) {
        if (DEBUG) Log.d(TAG, "Stopping service");
        context.stopServiceAsUser(new Intent(context, SamsungUDFPSService.class),
                UserHandle.CURRENT);
    }

    public static String getprop(String propName) {
        Process process = null;
        BufferedReader bufferedReader = null;

        try {
            if (DEBUG) Log.d(TAG, "getprop: get '" + propName + "'");

            process = new ProcessBuilder().command("getprop", propName).redirectErrorStream(true).start();
            bufferedReader = new BufferedReader(new InputStreamReader(process.getInputStream()));
            String line = bufferedReader.readLine();
            if (line == null){
                line = "";
                if (DEBUG) Log.d(TAG, "getprop: '" + propName + "' is empty");
            }
            else
            {
                if (DEBUG) Log.d(TAG, "getprop: '" + propName + "' is '" + line + "'");
            }
            return line;
        } catch (Exception e) {
            if (DEBUG) Log.d(TAG, "getprop: '" + propName + "' is empty");
            return "";
        } finally {
            if (bufferedReader != null){
                try {
                    bufferedReader.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            if (process != null){
                process.destroy();
            }
        }
    }

    public static void setprop(String prop, String value) {
        List<String> commands = new ArrayList<String>();
        commands.add("setprop");
        commands.add(prop);
        commands.add(value);

        ProcessBuilder pb = new ProcessBuilder(commands);
        pb.redirectErrorStream(true);

        try {
            Process prs = pb.start();
            OutputStream writeTo = prs.getOutputStream();

            writeTo.flush();
            writeTo.close();

        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}

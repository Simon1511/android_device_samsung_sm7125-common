/*
 * Copyright (C) 2023 The LineageOS Project
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

#include <android-base/properties.h>
#include <android-base/logging.h>
#include <android/hardware/audio/7.0/IDevicesFactory.h>

using android::base::GetProperty;
using android::base::SetProperty;
using android::sp;
using ::android::hardware::hidl_string;

int main() {
    sp<::android::hardware::audio::V7_0::IDevice> audioDevice;

    LOG(DEBUG) << "Passing g_call_sim_slot parameter";

    if (!audioDevice) {
        ::android::hardware::audio::V7_0::IDevicesFactory::getService()->openDevice(
            "primary", [&](::android::hardware::audio::V7_0::Result, const sp<::android::hardware::audio::V7_0::IDevice>& result) {
                    audioDevice = result;
        });
    }

    if (audioDevice != nullptr) {
        std::string value = "";

        if (strcmp(GetProperty("vendor.calls.slot_id1", "0").c_str(), "1") == 0) {
            // SIM 2
            value = "0x02";
        }

        audioDevice->setParameters({}, {{"g_call_sim_slot", value}});
    }

    if (audioDevice != nullptr)
        audioDevice->close();

    SetProperty("vendor.calls.parameter_state", "1");

    return 0;
}

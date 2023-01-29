/*
 * Copyright (C) 2018 The Android Open Source Project
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

#include "core/default/ParametersUtil.h"
#include "core/default/Util.h"

#include <system/audio.h>

#include <util/CoreUtils.h>

#include <tinyalsa/asoundlib.h>
#include <cutils/properties.h>

namespace android {
namespace hardware {
namespace audio {
namespace CORE_TYPES_CPP_VERSION {
namespace implementation {

/** Converts a status_t in Result according to the rules of AudioParameter::get*
 * Note: Static method and not private method to avoid leaking status_t dependency
 */
static Result getHalStatusToResult(status_t status) {
    switch (status) {
        case OK:
            return Result::OK;
        case BAD_VALUE:  // Nothing was returned, probably because the HAL does
                         // not handle it
            return Result::NOT_SUPPORTED;
        case INVALID_OPERATION:  // Conversion from string to the requested type
                                 // failed
            return Result::INVALID_ARGUMENTS;
        default:  // Should not happen
            ALOGW("Unexpected status returned by getParam: %u", status);
            return Result::INVALID_ARGUMENTS;
    }
}

Result ParametersUtil::getParam(const char* name, bool* value) {
    String8 halValue;
    Result retval = getParam(name, &halValue);
    *value = false;
    if (retval == Result::OK) {
        if (halValue.empty()) {
            return Result::NOT_SUPPORTED;
        }
        *value = !(halValue == AudioParameter::valueOff);
    }
    return retval;
}

Result ParametersUtil::getParam(const char* name, int* value) {
    const String8 halName(name);
    AudioParameter keys;
    keys.addKey(halName);
    std::unique_ptr<AudioParameter> params = getParams(keys);
    return getHalStatusToResult(params->getInt(halName, *value));
}

Result ParametersUtil::getParam(const char* name, String8* value, AudioParameter context) {
    const String8 halName(name);
    context.addKey(halName);
    std::unique_ptr<AudioParameter> params = getParams(context);
    return getHalStatusToResult(params->get(halName, *value));
}

void ParametersUtil::getParametersImpl(
    const hidl_vec<ParameterValue>& context, const hidl_vec<hidl_string>& keys,
    std::function<void(Result retval, const hidl_vec<ParameterValue>& parameters)> cb) {
    AudioParameter halKeys;
    for (auto& pair : context) {
        halKeys.add(String8(pair.key.c_str()), String8(pair.value.c_str()));
    }
    for (size_t i = 0; i < keys.size(); ++i) {
        halKeys.addKey(String8(keys[i].c_str()));
    }
    std::unique_ptr<AudioParameter> halValues = getParams(halKeys);
    Result retval =
        (keys.size() == 0 || halValues->size() != 0) ? Result::OK : Result::NOT_SUPPORTED;
    hidl_vec<ParameterValue> result;
    result.resize(halValues->size());
    String8 halKey, halValue;
    for (size_t i = 0; i < halValues->size(); ++i) {
        status_t status = halValues->getAt(i, halKey, halValue);
        if (status != OK) {
            result.resize(0);
            retval = getHalStatusToResult(status);
            break;
        }
        result[i].key = halKey.string();
        result[i].value = halValue.string();
    }
    cb(retval, result);
}

std::unique_ptr<AudioParameter> ParametersUtil::getParams(const AudioParameter& keys) {
    String8 paramsAndValues;
    char* halValues = halGetParameters(keys.keysToString().string());
    if (halValues != NULL) {
        paramsAndValues.setTo(halValues);
        free(halValues);
    } else {
        paramsAndValues.clear();
    }
    return std::unique_ptr<AudioParameter>(new AudioParameter(paramsAndValues));
}

Result ParametersUtil::setParam(const char* name, const char* value) {
    AudioParameter param;
    param.add(String8(name), String8(value));
    return setParams(param);
}

Result ParametersUtil::setParam(const char* name, bool value) {
    AudioParameter param;
    param.add(String8(name), String8(value ? AudioParameter::valueOn : AudioParameter::valueOff));
    return setParams(param);
}

Result ParametersUtil::setParam(const char* name, int value) {
    AudioParameter param;
    param.addInt(String8(name), value);
    return setParams(param);
}

Result ParametersUtil::setParam(const char* name, float value) {
    AudioParameter param;
    param.addFloat(String8(name), value);
    return setParams(param);
}

int getMixerValueByName(const char *name) {
    const auto mixer = mixer_open(0);
    const auto ctl = mixer_get_ctl_by_name(mixer, name);
    int value = 0;

    if (mixer == nullptr) {
        ALOGE("Failed to find mixer ctl for %s", name);
        return 0;
    }

    if (ctl == nullptr) {
        ALOGE("Failed to find mixer ctl for %s", name);
        return 0;
    }

    value = mixer_ctl_get_value(ctl, 0);

    mixer_close(mixer);

    return value;
}

const char *getMixerValueString(const char *name) {
    const auto mixer = mixer_open(0);
    const auto ctl = mixer_get_ctl_by_name(mixer, name);
    int control_value = mixer_ctl_get_value(ctl, 0);
    const char *value;

    if (mixer == nullptr) {
        ALOGE("Failed to find mixer ctl for %s", name);
        return "-1";
    }

    if (ctl == nullptr) {
        ALOGE("Failed to find mixer ctl for %s", name);
        return "-1";
    }

    value = mixer_ctl_get_enum_string(ctl, control_value);

    mixer_close(mixer);

    return value;
}

void setMixerValueByName(mixer *mixer, const char *name, int value) {
    const auto ctl = mixer_get_ctl_by_name(mixer, name);

    if (ctl == nullptr) {
        ALOGE("Failed to find mixer ctl for %s", name);
        return;
    }

    if (mixer_ctl_set_value(ctl, 0, value) < 0) {
        ALOGE("Failed to find mixer ctl for %s", name);
        return;
    }
}

void setMixerValueByNameString(mixer *mixer, const char *name, const char *value) {
    const auto ctl = mixer_get_ctl_by_name(mixer, name);

    if (ctl == nullptr) {
        ALOGE("Failed to find mixer ctl for %s", name);
        return;
    }

    if (mixer_ctl_set_enum_by_string(ctl, value)) {
        ALOGE("Failed to find mixer ctl for %s", name);
        return;
    }
}

void setA2DPMixers(bool enabled, char *slot) {
    const auto mixer = mixer_open(0);

    if (mixer == nullptr) {
        ALOGE("Failed to open mixer");
        return;
    }

    if (enabled) {
        ALOGD("Fixing A2DP mixers");
        setMixerValueByName(mixer, "QUIN_MI2S_RX_Voice Mixer VoiceMMode1", 0);
        setMixerValueByName(mixer, "QUIN_MI2S_RX_Voice Mixer VoiceMMode2", 0);
        setMixerValueByName(mixer, "VoiceMMode1_Tx Mixer TX_CDC_DMA_TX_3_MMode1", 0);
        setMixerValueByName(mixer, "VoiceMMode2_Tx Mixer TX_CDC_DMA_TX_3_MMode2", 0);
        setMixerValueByNameString(mixer, "SLIM7_RX ADM Channels", "Two");
        setMixerValueByName(mixer, "TAS256X IVSENSE ENABLE", 0);
        setMixerValueByName(mixer, "BT SOC status", 1);
        setMixerValueByName(mixer, "Voip_Tx Mixer TX_CDC_DMA_TX_3_Voip", 0);
        setMixerValueByName(mixer, "QUIN_MI2S_RX_Voice Mixer Voip", 0);
        setMixerValueByName(mixer, "TAS256X RECEIVER ENABLE LEFT", 0);
        setMixerValueByName(mixer, "SmartPA Mute", 1);
        setMixerValueByNameString(mixer, "TX_CDC_DMA_TX_3 Channels", "One");
        setMixerValueByName(mixer, "TAS256X ASI Left Switch", 0);
        setMixerValueByNameString(mixer, "TAS256X ASI1 SEL LEFT", "I2C offset");
        setMixerValueByName(mixer, "TAS256X AMP OUTPUT LVL LEFT", 20);
        setMixerValueByName(mixer, "Compress Gapless Playback", 1);
        setMixerValueByName(mixer, "TX_AIF1_CAP Mixer DEC0", 0);
        setMixerValueByName(mixer, "TX_AIF1_CAP Mixer DEC1", 0);
        setMixerValueByNameString(mixer, "TX SMIC MUX0", "ZERO");
        setMixerValueByNameString(mixer, "TX SMIC MUX1", "ZERO");
        setMixerValueByNameString(mixer, "TX DEC0 MUX", "MSM_DMIC");
        setMixerValueByNameString(mixer, "TX DEC1 MUX", "MSM_DMIC");
        setMixerValueByNameString(mixer, "EC Reference Bit Format", "S16_LE");
        setMixerValueByName(mixer, "ADC1 Volume", 12);
        setMixerValueByName(mixer, "ADC3 Volume", 12);
        setMixerValueByNameString(mixer, "BT SampleRate", "KHZ_16");
        setMixerValueByNameString(mixer, "BT SampleRate RX", "KHZ_16");
        setMixerValueByNameString(mixer, "BT SampleRate TX", "KHZ_16");
        setMixerValueByName(mixer, "ADC3_MIXER Switch", 0);

        if (strcmp(slot, "0") == 0) {
            setMixerValueByName(mixer, "SLIM_7_RX_Voice Mixer VoiceMMode1", 1);
            setMixerValueByName(mixer, "VoiceMMode1_Tx Mixer SLIM_7_TX_MMode1", 1);
            setMixerValueByName(mixer, "SLIM_7_RX_Voice Mixer VoiceMMode1", 0);
            setMixerValueByName(mixer, "VoiceMMode1_Tx Mixer SLIM_7_TX_MMode1", 0);
            setMixerValueByName(mixer, "SLIM_7_RX_Voice Mixer VoiceMMode1", 1);
            setMixerValueByName(mixer, "VoiceMMode1_Tx Mixer SLIM_7_TX_MMode1", 1);
        } else if (strcmp(slot, "1") == 0) {
            setMixerValueByName(mixer, "SLIM_7_RX_Voice Mixer VoiceMMode2", 1);
            setMixerValueByName(mixer, "VoiceMMode2_Tx Mixer SLIM_7_TX_MMode2", 1);
            setMixerValueByName(mixer, "SLIM_7_RX_Voice Mixer VoiceMMode2", 0);
            setMixerValueByName(mixer, "VoiceMMode2_Tx Mixer SLIM_7_TX_MMode2", 0);
            setMixerValueByName(mixer, "SLIM_7_RX_Voice Mixer VoiceMMode2", 1);
            setMixerValueByName(mixer, "VoiceMMode2_Tx Mixer SLIM_7_TX_MMode2", 1);
        }
    } else {
        ALOGD("Resetting A2DP mixers");
        setMixerValueByNameString(mixer, "SLIM7_RX ADM Channels", "Zero");
        setMixerValueByName(mixer, "TAS256X IVSENSE ENABLE", 1);
        setMixerValueByName(mixer, "TAS256X RECEIVER ENABLE LEFT", 1);
        setMixerValueByName(mixer, "SmartPA Mute", 0);
        setMixerValueByNameString(mixer, "TX_CDC_DMA_TX_3 Channels", "Two");
        setMixerValueByName(mixer, "TAS256X ASI Left Switch", 1);
        setMixerValueByNameString(mixer, "TAS256X ASI1 SEL LEFT", "LeftRightDiv2");
        setMixerValueByName(mixer, "TAS256X AMP OUTPUT LVL LEFT", 0);
        setMixerValueByName(mixer, "TX_AIF1_CAP Mixer DEC0", 1);
        setMixerValueByName(mixer, "TX_AIF1_CAP Mixer DEC1", 1);
        setMixerValueByNameString(mixer, "TX SMIC MUX0", "ADC0");
        setMixerValueByNameString(mixer, "TX SMIC MUX1", "ADC2");
        setMixerValueByNameString(mixer, "TX DEC0 MUX", "SWR_MIC");
        setMixerValueByNameString(mixer, "TX DEC1 MUX", "SWR_MIC");
        setMixerValueByName(mixer, "EC Reference Bit Format", 0);
        setMixerValueByName(mixer, "ADC1 Volume", 0);
        setMixerValueByName(mixer, "ADC3 Volume", 0);
        setMixerValueByName(mixer, "ADC3_MIXER Switch", 1);
        setMixerValueByName(mixer, "SLIM_7_RX_Voice Mixer VoiceMMode1", 0);
        setMixerValueByName(mixer, "VoiceMMode1_Tx Mixer SLIM_7_TX_MMode1", 0);
        setMixerValueByName(mixer, "SLIM_7_RX_Voice Mixer VoiceMMode2", 0);
        setMixerValueByName(mixer, "VoiceMMode2_Tx Mixer SLIM_7_TX_MMode2", 0);

        if (strcmp(slot, "0") == 0) {
            setMixerValueByName(mixer, "QUIN_MI2S_RX_Voice Mixer VoiceMMode1", 1);
            setMixerValueByName(mixer, "VoiceMMode1_Tx Mixer TX_CDC_DMA_TX_3_MMode1", 1);
            setMixerValueByName(mixer, "QUIN_MI2S_RX_Voice Mixer VoiceMMode1", 0);
            setMixerValueByName(mixer, "VoiceMMode1_Tx Mixer TX_CDC_DMA_TX_3_MMode1", 0);
            setMixerValueByName(mixer, "QUIN_MI2S_RX_Voice Mixer VoiceMMode1", 1);
            setMixerValueByName(mixer, "VoiceMMode1_Tx Mixer TX_CDC_DMA_TX_3_MMode1", 1);
        } else if (strcmp(slot, "1") == 0) {
            setMixerValueByName(mixer, "QUIN_MI2S_RX_Voice Mixer VoiceMMode2", 1);
            setMixerValueByName(mixer, "VoiceMMode2_Tx Mixer TX_CDC_DMA_TX_3_MMode2", 1);
            setMixerValueByName(mixer, "QUIN_MI2S_RX_Voice Mixer VoiceMMode2", 0);
            setMixerValueByName(mixer, "VoiceMMode2_Tx Mixer TX_CDC_DMA_TX_3_MMode2", 0);
            setMixerValueByName(mixer, "QUIN_MI2S_RX_Voice Mixer VoiceMMode2", 1);
            setMixerValueByName(mixer, "VoiceMMode2_Tx Mixer TX_CDC_DMA_TX_3_MMode2", 1);
        }
    }

    mixer_close(mixer);
}

void setEarpieceVoipMixers(bool enabled) {
    const auto mixer = mixer_open(0);

    if (mixer == nullptr) {
        ALOGE("Failed to open mixer");
        return;
    }

    if (enabled) {
        setMixerValueByNameString(mixer, "TX SMIC MUX0", "ADC0");
        setMixerValueByNameString(mixer, "TX SMIC MUX1", "ADC2");
        setMixerValueByName(mixer, "MultiMedia2 Mixer QUIN_MI2S_TX", 0);
        setMixerValueByNameString(mixer, "TAS256X ASI1 SEL RIGHT", "I2C offset");
        setMixerValueByName(mixer, "TAS256X ASI Right Switch", 0);
        setMixerValueByNameString(mixer, "TAS256X ASI1 SEL LEFT", "LeftRightDiv2");
        setMixerValueByName(mixer, "TAS256X RECEIVER ENABLE LEFT", 1);
        setMixerValueByName(mixer, "TAS256X ASI Left Switch", 1);
        setMixerValueByName(mixer, "TAS256X AMP OUTPUT LVL LEFT", 8);
        setMixerValueByName(mixer, "TAS256X PLAYBACK VOLUME LEFT", 55);
        setMixerValueByName(mixer, "TAS256X PLAYBACK VOLUME RIGHT", 55);
        setMixerValueByName(mixer, "SmartPA Mute", 0);
        setMixerValueByNameString(mixer, "EC Reference Channels", "One");
        setMixerValueByNameString(mixer, "AUDIO_REF_EC_UL10 MUX", "QUIN_MI2S_RX");
        setMixerValueByName(mixer, "QUIN_MI2S_RX_Voice Mixer Voip", 1);
        setMixerValueByName(mixer, "Voip_Tx Mixer TX_CDC_DMA_TX_3_Voip", 1);
        setMixerValueByName(mixer, "QUIN_MI2S_RX_Voice Mixer Voip", 0);
        setMixerValueByName(mixer, "Voip_Tx Mixer TX_CDC_DMA_TX_3_Voip", 0);
    }

    mixer_close(mixer);
}

void setSpeakerVoipMixers(bool enabled) {
    const auto mixer = mixer_open(0);

    if (mixer == nullptr) {
        ALOGE("Failed to open mixer");
        return;
    }

    if (enabled) {
        setMixerValueByNameString(mixer, "TX SMIC MUX0", "ADC2");
        setMixerValueByNameString(mixer, "TX SMIC MUX1", "ADC0");
        setMixerValueByName(mixer, "MultiMedia2 Mixer QUIN_MI2S_TX", 1);
        setMixerValueByNameString(mixer, "TAS256X ASI1 SEL RIGHT", "Right");
        setMixerValueByName(mixer, "TAS256X ASI Right Switch", 1);
        setMixerValueByNameString(mixer, "TAS256X ASI1 SEL LEFT", "Left");
        setMixerValueByName(mixer, "TAS256X RECEIVER ENABLE LEFT", 0);
        setMixerValueByName(mixer, "TAS256X ASI Left Switch", 1);
        setMixerValueByName(mixer, "TAS256X AMP OUTPUT LVL LEFT", 20);
        setMixerValueByName(mixer, "TAS256X PLAYBACK VOLUME LEFT", 55);
        setMixerValueByName(mixer, "TAS256X PLAYBACK VOLUME RIGHT", 55);
        setMixerValueByName(mixer, "SmartPA Mute", 0);
        setMixerValueByNameString(mixer, "EC Reference Channels", "Two");
        setMixerValueByNameString(mixer, "AUDIO_REF_EC_UL10 MUX", "QUIN_MI2S_RX");
        setMixerValueByName(mixer, "QUIN_MI2S_RX_Voice Mixer Voip", 1);
        setMixerValueByName(mixer, "Voip_Tx Mixer TX_CDC_DMA_TX_3_Voip", 1);
        setMixerValueByName(mixer, "QUIN_MI2S_RX_Voice Mixer Voip", 0);
        setMixerValueByName(mixer, "Voip_Tx Mixer TX_CDC_DMA_TX_3_Voip", 0);
    }

    mixer_close(mixer);
}

void fixupMixers(int state) {
    char a2dpcall[92];
    char simslot[92];
    char voipdevice[92];
    char callswitched[92];
    const char *ampstatus;
    int voipstatus;

    property_get("vendor.audio.a2dp.connected", a2dpcall, "false");
    property_get("vendor.calls.slotid", simslot, "-1");
    property_get("vendor.audio.voip.device", voipdevice, "none");
    property_get("vendor.audio.call.switched", callswitched, "false");

    voipstatus = getMixerValueByName("QUIN_MI2S_RX_Voice Mixer Voip");
    ampstatus = getMixerValueString("TAS256X RECEIVER ENABLE LEFT");

    ALOGD("ampstatus is %s", ampstatus);
    ALOGD("voipstatus is %d", voipstatus);

    // A2DP call
    if (strcmp(simslot, "0") == 0 || strcmp(simslot, "1") == 0) {
        ALOGD("In phone call");
        if (strcmp(a2dpcall, "true") == 0 && state == 1) {
            ALOGD("In A2DP call");
            setA2DPMixers(true, simslot);
        } else if (strcmp(a2dpcall, "true") == 0 && state == 2 && strcmp(callswitched, "true") == 0) {
            setA2DPMixers(false, simslot);
        }
    }

    // VoIP call
    if (voipstatus == 1) {
        ALOGE("In VoIP call");
        if (strcmp(voipdevice, "earpiece") == 0 && strcmp(ampstatus, "DISABLE") == 0) {
            ALOGD("Setting earpiece mixers");
            setEarpieceVoipMixers(true);
        }

        if (strcmp(voipdevice, "speaker") == 0 && strcmp(ampstatus, "ENABLE") == 0) {
            ALOGD("Setting speaker mixers");
            setSpeakerVoipMixers(true);
        }
    }
}

Result ParametersUtil::setParametersImpl(const hidl_vec<ParameterValue>& context,
                                         const hidl_vec<ParameterValue>& parameters) {
    AudioParameter params;
    for (auto& pair : context) {
        params.add(String8(pair.key.c_str()), String8(pair.value.c_str()));
    }
    for (size_t i = 0; i < parameters.size(); ++i) {
        if (parameters[i].key == "bt_wbs") {
            params.add(String8("g_sco_samplerate"),
                       String8(parameters[i].value == AudioParameter::valueOn ? "16000" : "8000"));
        }
        if (parameters[i].key == "a2dp_call") {
            if (parameters[i].value == "1") {
                fixupMixers(1);
            }
        }
        if (parameters[i].key == "voip_call") {
            if (parameters[i].value == "1") {
                fixupMixers(0);
            }
        }
        // always true when a call is started and output device changes to BT in a call
        if (parameters[i].key == "A2dpSuspended") {
            if (parameters[i].value == "true") {
                fixupMixers(1);
            }
        }
        if (parameters[i].key == "BT_SCO") {
            if (parameters[i].value == "off") {
                fixupMixers(2);
            }
        }
        params.add(String8(parameters[i].key.c_str()), String8(parameters[i].value.c_str()));
    }
    return setParams(params);
}

Result ParametersUtil::setParam(const char* name, const DeviceAddress& address) {
    audio_devices_t halDeviceType;
    char halDeviceAddress[AUDIO_DEVICE_MAX_ADDRESS_LEN];
    if (CoreUtils::deviceAddressToHal(address, &halDeviceType, halDeviceAddress) != NO_ERROR) {
        return Result::INVALID_ARGUMENTS;
    }
    if (halDeviceType == AUDIO_DEVICE_OUT_BLUETOOTH_A2DP || halDeviceType == AUDIO_DEVICE_OUT_BLUETOOTH_SCO
        || halDeviceType == AUDIO_DEVICE_IN_BLUETOOTH_SCO_HEADSET) {
        if (strcmp(name, "connect") == 0) {
            property_set("vendor.audio.a2dp.connected", "true");
        } else if (strcmp(name, "disconnect") == 0) {
            property_set("vendor.audio.a2dp.connected", "false");
        }
    }
    AudioParameter params{String8(halDeviceAddress)};
    params.addInt(String8(name), halDeviceType);
    return setParams(params);
}

Result ParametersUtil::setParams(const AudioParameter& param) {
    int halStatus = halSetParameters(param.toString().string());
    return util::analyzeStatus(halStatus);
}

}  // namespace implementation
}  // namespace CORE_TYPES_CPP_VERSION
}  // namespace audio
}  // namespace hardware
}  // namespace android

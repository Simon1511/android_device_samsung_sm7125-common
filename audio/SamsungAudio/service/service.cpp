/* tinymix.c
**
** Copyright 2011, The Android Open Source Project
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**     * Neither the name of The Android Open Source Project nor the names of
**       its contributors may be used to endorse or promote products derived
**       from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY The Android Open Source Project ``AS IS'' AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL The Android Open Source Project BE LIABLE
** FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
** DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
** CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
** LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
** OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
** DAMAGE.
*/

#define LOG_TAG "SamsungAudioService"

#include <tinyalsa/asoundlib.h>
#include <android-base/logging.h>
#include <cutils/properties.h>
#include <string.h>
#include <chrono>
#include <thread>

int getMixerValueByName(const char *name) {
    const auto mixer = mixer_open(0);
    const auto ctl = mixer_get_ctl_by_name(mixer, name);
    int value = 0;

    if (mixer == nullptr) {
        LOG(ERROR) << "Failed to open mixer";
        return 0;
    }

    if (ctl == nullptr) {
        LOG(ERROR) << "Failed to find mixer ctl for %s" << name;
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
        LOG(ERROR) << "Failed to open mixer";
        return "-1";
    }

    if (ctl == nullptr) {
        LOG(ERROR) << "Failed to find mixer ctl for %s" << name;
        return "-1";
    }

    value = mixer_ctl_get_enum_string(ctl, control_value);

    mixer_close(mixer);

    return value;
}

void setMixerValueByName(mixer *mixer, const char *name, int value) {
    const auto ctl = mixer_get_ctl_by_name(mixer, name);

    if (ctl == nullptr) {
        LOG(ERROR) << "Failed to find mixer ctl for %s" << name;
        return;
    }

    if (mixer_ctl_set_value(ctl, 0, value) < 0) {
        LOG(ERROR) << "Failed to find mixer ctl for %s" << name;
        return;
    }
}

void setMixerValueByNameString(mixer *mixer, const char *name, const char *value) {
    const auto ctl = mixer_get_ctl_by_name(mixer, name);

    if (ctl == nullptr) {
        LOG(ERROR) << "Failed to find mixer ctl for %s" << name;
        return;
    }

    if (mixer_ctl_set_enum_by_string(ctl, value)) {
        LOG(ERROR) << "Failed to find mixer ctl for %s" << name;
        return;
    }
}

void setA2DPMixers(bool enabled, char *slot) {
    const auto mixer = mixer_open(0);

    if (mixer == nullptr) {
        LOG(ERROR) << "Failed to open mixer";
        return;
    }

    if (enabled) {
        LOG(ERROR) << "Fixing A2DP mixers";
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
    }

    mixer_close(mixer);
}

void setEarpieceVoipMixers(bool enabled) {
    const auto mixer = mixer_open(0);

    if (mixer == nullptr) {
        LOG(ERROR) << "Failed to open mixer";
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
        LOG(ERROR) << "Failed to open mixer";
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

int main() {
    char a2dpcall[92];
    char simslot[92];
    const char *ampstatus;
    int voipstatus;
    char epcstatus[92];
    char spkstatus[92];

    LOG(ERROR) << "SamsungAudioService started";

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        property_get("vendor.audio.a2dp.call", a2dpcall, "false");
        property_get("vendor.calls.slotid", simslot, "-1");
        property_get("vendor.audio.earpiece.voip", epcstatus, "0");
        property_get("vendor.audio.speaker.voip", spkstatus, "0");

        voipstatus = getMixerValueByName("QUIN_MI2S_RX_Voice Mixer Voip");
        ampstatus = getMixerValueString("TAS256X RECEIVER ENABLE LEFT");

        LOG(ERROR) << "ampstatus is " << ampstatus;

        // A2DP call
        if (strcmp(simslot, "0") == 0 || strcmp(simslot, "1") == 0) {
            LOG(ERROR) << "In phone call";
            if (strcmp(a2dpcall, "true") == 0) {
                LOG(ERROR) << "In A2DP call";
                setA2DPMixers(true, simslot);
            }
        }

        // VoIP call
        if (voipstatus == 1) {
            LOG(ERROR) << "In VoIP call";

            if (strcmp(epcstatus, "1") == 0 && strcmp(spkstatus, "0") == 0 && strcmp(ampstatus, "DISABLE") == 0) {
                setEarpieceVoipMixers(true);
            }

            if (strcmp(epcstatus, "0") == 0 && strcmp(spkstatus, "1") == 0 && strcmp(ampstatus, "ENABLE") == 0) {
                setSpeakerVoipMixers(true);
            }
        }
    }

    return 0;
}

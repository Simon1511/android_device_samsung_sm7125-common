#! /vendor/bin/sh

# Call as follows: samsungAudio.sh <param>
# Where <param> is either voip-speaker, voip-handset or a2dp

mode=$1

# Earpiece to BT
if [[ "$mode" == "a2dp" ]]; then
	/system/bin/tinymix "SLIM_7_RX_Voice Mixer VoiceMMode1" 0
	/system/bin/tinymix "VoiceMMode1_Tx Mixer SLIM_7_TX_MMode1" 0
	/system/bin/tinymix "TAS256X PLAYBACK VOLUME LEFT" 0
	/system/bin/tinymix "TAS256X PLAYBACK VOLUME RIGHT" 0
	/system/bin/tinymix "SLIM7_RX ADM Channels" Two
	/system/bin/tinymix "SLIM_7_RX_Voice Mixer VoiceMMode1" 1
	/system/bin/tinymix "VoiceMMode1_Tx Mixer SLIM_7_TX_MMode1" 1
fi

# VOIP handset
if [[ "$mode" == "voip-handset" ]]; then
	/system/bin/tinymix "TAS256X ASI1 SEL RIGHT" "I2C offset"
	/system/bin/tinymix "TAS256X ASI Right Switch" 0

	/system/bin/tinymix "TAS256X ASI1 SEL LEFT" LeftRightDiv2
	/system/bin/tinymix "TAS256X RECEIVER ENABLE LEFT" ENABLE
	/system/bin/tinymix "TAS256X ASI Left Switch" 1
	/system/bin/tinymix "TAS256X AMP OUTPUT LVL LEFT" 8
	/system/bin/tinymix "TAS256X PLAYBACK VOLUME LEFT" 55
	/system/bin/tinymix "TAS256X PLAYBACK VOLUME RIGHT" 0
	/system/bin/tinymix "SmartPA Mute" 0

	/system/bin/tinymix "EC Reference Channels" One
	/system/bin/tinymix "AUDIO_REF_EC_UL10 MUX" QUIN_MI2S_RX

	/system/bin/tinymix "QUIN_MI2S_RX_Voice Mixer Voip" 1
	/system/bin/tinymix "Voip_Tx Mixer TX_CDC_DMA_TX_3_Voip" 1
fi

# VOIP speaker
if [[ "$mode" == "voip-speaker" ]]; then
	/system/bin/tinymix "TAS256X ASI1 SEL LEFT" "I2C offset"
	/system/bin/tinymix "TAS256X RECEIVER ENABLE LEFT" DISABLE
	/system/bin/tinymix "TAS256X ASI Left Switch" 0
	/system/bin/tinymix "TAS256X AMP OUTPUT LVL LEFT" 0
	/system/bin/tinymix "TAS256X PLAYBACK VOLUME LEFT" 0

	/system/bin/tinymix "TAS256X ASI1 SEL RIGHT" LeftRightDiv2
	/system/bin/tinymix "TAS256X ASI Right Switch" 1

	/system/bin/tinymix "TX_CDC_DMA_TX_3 Channels" Two
	/system/bin/tinymix "TX DEC0 MUX" SWR_MIC
	/system/bin/tinymix "TX SMIC MUX0" ADC2
	/system/bin/tinymix "TX_AIF1_CAP Mixer DEC0" 1
	/system/bin/tinymix "ADC3_MIXER Switch" 1
	/system/bin/tinymix "ADC3 MUX" INP4
	/system/bin/tinymix "TX DEC1 MUX" SWR_MIC
	/system/bin/tinymix "TX SMIC MUX1" ADC0
	/system/bin/tinymix "TX_AIF1_CAP Mixer DEC1" 1
	/system/bin/tinymix "ADC1_MIXER Switch" 1
	/system/bin/tinymix "TX_DEC0 Volume" 96
	/system/bin/tinymix "TX_DEC1 Volume" 90
	/system/bin/tinymix "ADC1 Volume" 0
	/system/bin/tinymix "ADC3 Volume" 0

	/system/bin/tinymix "EC Reference Channels" Two
	/system/bin/tinymix "AUDIO_REF_EC_UL1 MUX" QUIN_MI2S_RX

	/system/bin/tinymix "QUIN_MI2S_RX_Voice Mixer Voip" 1
	/system/bin/tinymix "Voip_Tx Mixer TX_CDC_DMA_TX_3_Voip" 1
fi

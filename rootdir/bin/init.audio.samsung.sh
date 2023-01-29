#! /vendor/bin/sh

# Call as follows: samsungAudio.sh <param>
# Where <param> is either voip-speaker, voip-handset or a2dp

mode=$1
tinymix=/system/bin/tinymix

/system/bin/log -t VoIPCallFix "Starting"

while true
do
	voipstatus=$($tinymix "QUIN_MI2S_RX_Voice Mixer Voip" --v | awk '{ print $1 }')
	# "DISABLE" == speaker, "ENABLE" == earpiece
	ampstatus=$($tinymix "TAS256X RECEIVER ENABLE LEFT" --v | awk '{ print $1 }')

	output=$(/system/bin/dumpsys audio | grep 'Active communication device')

	/system/bin/log -t VoIPCallFix "VoIP status is $voipstatus"
	/system/bin/log -t VoIPCallFix "ampstatus status is $ampstatus"

	if [[ $voipstatus == "On" ]]; then
		/system/bin/log -t VoIPCallFix "In VoIP"
		if echo $output | grep "earpiece" && [[ $ampstatus == "DISABLE" ]]; then
			# earpiece
			/system/bin/log -t VoIPCallFix "Setting Earpiece ctls"

			$tinymix "TX SMIC MUX0" ADC0
			$tinymix "TX SMIC MUX1" ADC2
			$tinymix "MultiMedia2 Mixer QUIN_MI2S_TX" 0
			$tinymix "TAS256X ASI1 SEL RIGHT" "I2C offset"
			$tinymix "TAS256X ASI Right Switch" 0
			$tinymix "TAS256X ASI1 SEL LEFT" LeftRightDiv2
			$tinymix "TAS256X RECEIVER ENABLE LEFT" 1
			$tinymix "TAS256X ASI Left Switch" 1
			$tinymix "TAS256X AMP OUTPUT LVL LEFT" 8
			$tinymix "TAS256X PLAYBACK VOLUME LEFT" 55
			$tinymix "TAS256X PLAYBACK VOLUME RIGHT" 55
			$tinymix "SmartPA Mute" 0
			$tinymix "EC Reference Channels" One
			$tinymix "AUDIO_REF_EC_UL10 MUX" QUIN_MI2S_RX
			# For somehow need to turn these on and off to get audio output
			$tinymix "QUIN_MI2S_RX_Voice Mixer Voip" 1
			$tinymix "Voip_Tx Mixer TX_CDC_DMA_TX_3_Voip" 1
			$tinymix "QUIN_MI2S_RX_Voice Mixer Voip" 0
			$tinymix "Voip_Tx Mixer TX_CDC_DMA_TX_3_Voip" 0
		elif echo $output | grep "speaker" && [[ $ampstatus == "ENABLE" ]]; then
			/system/bin/log -t VoIPCallFix "Setting Speaker ctls"

			$tinymix "TAS256X RECEIVER ENABLE LEFT" 0
		fi
	fi

	sleep 2
done

# Earpiece to BT
if [[ "$mode" == "a2dp" ]]; then
	$tinymix "QUIN_MI2S_RX_Voice Mixer VoiceMMode1" 0
	$tinymix "VoiceMMode1_Tx Mixer TX_CDC_DMA_TX_3_MMode1" 0
#	$tinymix "SLIM_7_RX_Voice Mixer VoiceMMode1" 0
#	$tinymix "VoiceMMode1_Tx Mixer SLIM_7_TX_MMode1" 0
	$tinymix "TAS256X PLAYBACK VOLUME LEFT" 0
	$tinymix "TAS256X PLAYBACK VOLUME RIGHT" 0
	$tinymix "SLIM7_RX ADM Channels" Two
	$tinymix "SLIM_7_RX_Voice Mixer VoiceMMode1" 1
	$tinymix "VoiceMMode1_Tx Mixer SLIM_7_TX_MMode1" 1
fi

# VOIP handset
if [[ "$mode" == "voip-handset" ]]; then
	$tinymix "TAS256X ASI1 SEL RIGHT" "I2C offset"
	$tinymix "TAS256X ASI Right Switch" 0

	$tinymix "TAS256X ASI1 SEL LEFT" LeftRightDiv2
	$tinymix "TAS256X RECEIVER ENABLE LEFT" ENABLE
	$tinymix "TAS256X ASI Left Switch" 1
	$tinymix "TAS256X AMP OUTPUT LVL LEFT" 8
	$tinymix "TAS256X PLAYBACK VOLUME LEFT" 55
	$tinymix "TAS256X PLAYBACK VOLUME RIGHT" 0
	$tinymix "SmartPA Mute" 0

	$tinymix "EC Reference Channels" One
	$tinymix "AUDIO_REF_EC_UL10 MUX" QUIN_MI2S_RX

	$tinymix "QUIN_MI2S_RX_Voice Mixer Voip" 1
	$tinymix "Voip_Tx Mixer TX_CDC_DMA_TX_3_Voip" 1
fi

# VOIP speaker
if [[ "$mode" == "voip-speaker" ]]; then
	$tinymix "TAS256X ASI1 SEL LEFT" "I2C offset"
	$tinymix "TAS256X RECEIVER ENABLE LEFT" DISABLE
	$tinymix "TAS256X ASI Left Switch" 0
	$tinymix "TAS256X AMP OUTPUT LVL LEFT" 0
	$tinymix "TAS256X PLAYBACK VOLUME LEFT" 0

	$tinymix "TAS256X ASI1 SEL RIGHT" LeftRightDiv2
	$tinymix "TAS256X ASI Right Switch" 1

	$tinymix "TX_CDC_DMA_TX_3 Channels" Two
	$tinymix "TX DEC0 MUX" SWR_MIC
	$tinymix "TX SMIC MUX0" ADC2
	$tinymix "TX_AIF1_CAP Mixer DEC0" 1
	$tinymix "ADC3_MIXER Switch" 1
	$tinymix "ADC3 MUX" INP4
	$tinymix "TX DEC1 MUX" SWR_MIC
	$tinymix "TX SMIC MUX1" ADC0
	$tinymix "TX_AIF1_CAP Mixer DEC1" 1
	$tinymix "ADC1_MIXER Switch" 1
	$tinymix "TX_DEC0 Volume" 96
	$tinymix "TX_DEC1 Volume" 90
	$tinymix "ADC1 Volume" 0
	$tinymix "ADC3 Volume" 0

	$tinymix "EC Reference Channels" Two
	$tinymix "AUDIO_REF_EC_UL1 MUX" QUIN_MI2S_RX

	$tinymix "QUIN_MI2S_RX_Voice Mixer Voip" 1
	$tinymix "Voip_Tx Mixer TX_CDC_DMA_TX_3_Voip" 1
fi

#! /vendor/bin/sh

tinymix=/system/bin/tinymix
log=/system/bin/log

$log -t VoIPCallFix "Starting"

while true
do
	voipstatus=$($tinymix "QUIN_MI2S_RX_Voice Mixer Voip" --v | awk '{ print $1 }')
	# "DISABLE" == speaker, "ENABLE" == earpiece
	ampstatus=$($tinymix "TAS256X RECEIVER ENABLE LEFT" --v | awk '{ print $1 }')

	output=$(/system/bin/dumpsys audio | grep 'Active communication device')

	$log -t VoIPCallFix "VoIP status is $voipstatus"
	$log -t VoIPCallFix "ampstatus status is $ampstatus"

	if [[ $voipstatus == "On" ]]; then
		$log -t VoIPCallFix "In VoIP"
		if echo $output | grep "earpiece" && [[ $ampstatus == "DISABLE" ]]; then
			# earpiece
			$log -t VoIPCallFix "Setting Earpiece ctls"

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
			$log -t VoIPCallFix "Setting Speaker ctls"

			$tinymix "TX SMIC MUX0" ADC2
			$tinymix "TX SMIC MUX1" ADC0
			$tinymix "MultiMedia2 Mixer QUIN_MI2S_TX" 1
			$tinymix "TAS256X ASI1 SEL RIGHT" Right
			$tinymix "TAS256X ASI Right Switch" 1
			$tinymix "TAS256X ASI1 SEL LEFT" Left
			$tinymix "TAS256X RECEIVER ENABLE LEFT" 0
			$tinymix "TAS256X ASI Left Switch" 1
			$tinymix "TAS256X AMP OUTPUT LVL LEFT" 20
			$tinymix "TAS256X PLAYBACK VOLUME LEFT" 55
			$tinymix "TAS256X PLAYBACK VOLUME RIGHT" 55
			$tinymix "SmartPA Mute" 0
			$tinymix "EC Reference Channels" Two
			$tinymix "AUDIO_REF_EC_UL10 MUX" QUIN_MI2S_RX
			# For somehow need to turn these on and off to get audio output
			$tinymix "QUIN_MI2S_RX_Voice Mixer Voip" 1
			$tinymix "Voip_Tx Mixer TX_CDC_DMA_TX_3_Voip" 1
			$tinymix "QUIN_MI2S_RX_Voice Mixer Voip" 0
			$tinymix "Voip_Tx Mixer TX_CDC_DMA_TX_3_Voip" 0
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

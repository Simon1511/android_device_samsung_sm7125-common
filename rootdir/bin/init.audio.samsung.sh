#! /vendor/bin/sh

while [ "$(getprop sys.boot_completed | tr -d '\r')" != "1" ]; do sleep 1; done
sleep 10

tinymix=/system/bin/tinymix
log=/system/bin/log
setprop=/system/bin/setprop
getprop=/system/bin/getprop

$log -t SamsungAudio "Starting"

while true
do
	# VoIP status
	voipstatus=$($tinymix "QUIN_MI2S_RX_Voice Mixer Voip" --v | awk '{ print $1 }')
	# Call status
	callstatus=$($getprop vendor.calls.slotid)
	# A2DP call state
	a2dpstate=$(/system/bin/dumpsys audio | grep mScoAudioState)
	# AMP status: "DISABLE" == speaker, "ENABLE" == earpiece
	ampstatus=$($tinymix "TAS256X RECEIVER ENABLE LEFT" --v | awk '{ print $1 }')

	# Earpiece/Speaker VoIP state
	epc=$($getprop vendor.audio.earpiece.voip)
	spk=$($getprop vendor.audio.speaker.voip)

	$log -t SamsungAudio "VoIP status is $voipstatus"
	$log -t SamsungAudio "ampstatus status is $ampstatus"

	# VoIP
	if [[ $voipstatus == "On" ]]; then
		$log -t SamsungAudio "In VoIP call"
		if [[ $epc == "true" ]] && [[ $spk == "false" ]] && [[ $ampstatus == "DISABLE" ]]; then
			# Earpiece
			$log -t SamsungAudio "Setting Earpiece ctls"

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
		elif [[ $epc == "false" ]] && [[ $spk == "true" ]] && [[ $ampstatus == "ENABLE" ]]; then
			# Speaker
			$log -t SamsungAudio "Setting Speaker ctls"

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

	# A2DP call
	if [[ $callstatus == "0" ]] || [[ $callstatus == "1" ]]; then
		$log -t SamsungAudio "In phone call"
		if echo $a2dpstate | grep "SCO_STATE_ACTIVE_EXTERNAL" && [[ $ampstatus == "ENABLE" ]]; then
			$log -t SamsungAudio "In A2DP phone call, setting A2DP ctls"

			$tinymix "QUIN_MI2S_RX_Voice Mixer VoiceMMode1" 0
			$tinymix "QUIN_MI2S_RX_Voice Mixer VoiceMMode2" 0
			$tinymix "VoiceMMode1_Tx Mixer TX_CDC_DMA_TX_3_MMode1" 0
			$tinymix "VoiceMMode2_Tx Mixer TX_CDC_DMA_TX_3_MMode2" 0
			$tinymix "SLIM7_RX ADM Channels" Two
			$tinymix "TAS256X IVSENSE ENABLE" 0
			$tinymix "BT SOC status" 1
			$tinymix "Voip_Tx Mixer TX_CDC_DMA_TX_3_Voip" 0
			$tinymix "QUIN_MI2S_RX_Voice Mixer Voip" 0
			$tinymix "TAS256X RECEIVER ENABLE LEFT" 0
			$tinymix "SmartPA Mute" 1
			$tinymix "TX_CDC_DMA_TX_3 Channels" One
			$tinymix "TAS256X ASI Left Switch" 0
			$tinymix "TAS256X ASI1 SEL LEFT" "I2C offset"
			$tinymix "TAS256X AMP OUTPUT LVL LEFT" 20
			$tinymix "Compress Gapless Playback" 1
			$tinymix "TX_AIF1_CAP Mixer DEC0" 0
			$tinymix "TX_AIF1_CAP Mixer DEC1" 0
			$tinymix "TX SMIC MUX0" ZERO
			$tinymix "TX SMIC MUX1" ZERO
			$tinymix "TX DEC0 MUX" MSM_DMIC
			$tinymix "TX DEC1 MUX" MSM_DMIC
			$tinymix "EC Reference Bit Format" "S16_LE"
			$tinymix "ADC1 Volume" 12
			$tinymix "ADC3 Volume" 12
			$tinymix "BT SampleRate" "KHZ_16"
			$tinymix "BT SampleRate RX" "KHZ_16"
			$tinymix "BT SampleRate TX" "KHZ_16"
			$tinymix "ADC3_MIXER Switch" 0

			# SIM 1 or SIM 2
			if [[ $callstatus == "0" ]]; then
				$tinymix "SLIM_7_RX_Voice Mixer VoiceMMode1" 1
				$tinymix "VoiceMMode1_Tx Mixer SLIM_7_TX_MMode1" 1
				# For somehow need to turn these on and off to get audio output
				$tinymix "SLIM_7_RX_Voice Mixer VoiceMMode1" 0
				$tinymix "VoiceMMode1_Tx Mixer SLIM_7_TX_MMode1" 0
				$tinymix "SLIM_7_RX_Voice Mixer VoiceMMode1" 1
				$tinymix "VoiceMMode1_Tx Mixer SLIM_7_TX_MMode1" 1
			elif [[ $callstatus == "1" ]]; then
				$tinymix "SLIM_7_RX_Voice Mixer VoiceMMode2" 1
				$tinymix "VoiceMMode2_Tx Mixer SLIM_7_TX_MMode2" 1
				# For somehow need to turn these on and off to get audio output
				$tinymix "SLIM_7_RX_Voice Mixer VoiceMMode2" 0
				$tinymix "VoiceMMode2_Tx Mixer SLIM_7_TX_MMode2" 0
				$tinymix "SLIM_7_RX_Voice Mixer VoiceMMode2" 1
				$tinymix "VoiceMMode2_Tx Mixer SLIM_7_TX_MMode2" 1
			fi
		fi
	fi

	sleep 2
done

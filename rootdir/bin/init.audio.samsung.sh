#! /vendor/bin/sh

while [ "$(getprop sys.boot_completed | tr -d '\r')" != "1" ]; do sleep 1; done
sleep 10

setprop=/vendor/bin/setprop
getprop=/vendor/bin/getprop

while true
do
	# Call status
	callstatus=$($getprop vendor.calls.slotid)
	# A2DP call state
	a2dpstate=$(/vendor/bin/dumpsys audio | grep mScoAudioState)

	/vendor/bin/log -t SamsungAudio "running, a2dpstate is $a2dpstate, callstatus is $callstatus"

	# A2DP call
	if [[ $callstatus == "0" ]] || [[ $callstatus == "1" ]]; then
		if echo $a2dpstate | grep "SCO_STATE_ACTIVE_EXTERNAL"; then
			$setprop "vendor.audio.a2p.call" "true"
		elif
			$setprop "vendor.audio.a2p.call" "false"
		fi
	fi

	sleep 2
done

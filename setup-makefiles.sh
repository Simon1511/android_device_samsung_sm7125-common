#!/bin/bash
#
# Copyright (C) 2017-2021 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

set -e

DEVICE_COMMON=sm7125-common
VENDOR=samsung

# Load extractutils and do some sanity checks
MY_DIR="${BASH_SOURCE%/*}"
if [[ ! -d "$MY_DIR" ]]; then MY_DIR="$PWD"; fi

ANDROID_ROOT="${MY_DIR}/../../.."

HELPER="${ANDROID_ROOT}/tools/extract-utils/extract_utils.sh"
if [ ! -f "${HELPER}" ]; then
    echo "Unable to find helper script at ${HELPER}"
    exit 1
fi
source "${HELPER}"

# Initialize the helper
setup_vendor "${DEVICE_COMMON}" "${VENDOR}" "${ANDROID_ROOT}" true

# Warning headers and guards
write_headers "a52q a72q"

write_makefiles "${MY_DIR}/proprietary-files.txt" true

###################################################################################################
# CUSTOM PART START                                                                               #
###################################################################################################

OUTDIR=vendor/$VENDOR/$DEVICE_COMMON

###################################################################################################
# CUSTOM PART END                                                                                 #
###################################################################################################
# Done
write_footers

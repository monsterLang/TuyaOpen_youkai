#!/bin/bash

COLOUR_GREEN="\033[32m"
NO_COLOUR="\033[0m"

start_time=`date +%s`
build_start_time=`date +%H:%M:%S`

# build

. ./export.sh
tos.py version
tos.py check

cd apps/tuya_cloud/01_i2c
echo 9 | tos.py config choice

tos.py build
# tos.py clean
deactivate


build_end_time=`date +%H:%M:%S`
time_end=`date +%s`

time_total=`expr $(( $time_end - $start_time))`
time_hour=`expr $(($time_total / 3600))`
time_min=`expr $(($time_total % 3600 / 60))`
time_sec=`expr $(($time_total % 60))`
build_total_time=`printf "%02d:%02d:%02d\n" $time_hour $time_min $time_sec`


echo "===================================================================="
echo "Start time: $build_start_time"
echo "End time: $build_end_time"
echo -e "Total time: ${COLOUR_GREEN} $build_total_time ${NO_COLOUR}"
echo "===================================================================="




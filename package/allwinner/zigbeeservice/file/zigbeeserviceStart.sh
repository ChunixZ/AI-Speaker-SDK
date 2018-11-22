#!/bin/sh


sleep 3

while :
do
    #DATE=$(date +%Y-%m-%d-%T).txt
    stillRunning=$(ps -w | grep -w "zigbeeService" |grep -v "grep")
    if [ "$stillRunning" ] ; then
        echo "stillRunning"
    else
        if [ "$(ls /dev/ | grep -w "ttyS2")" ] ; then
            /home/zigbeeService & #/dev/ttyS2 #> /home/log/$DATE
        else
            /home/zigbeeService & #/dev/ttyS2 #> /home/log/$DATE
        fi
    fi
    sleep 5
done

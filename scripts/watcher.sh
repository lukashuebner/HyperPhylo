#!/bin/bash

LIST_OF_HOSTS="128 129 132"
if [ -n $1 ]; then
	LIST_OF_HOSTS="$1"
fi

success=1
while true; do
    for host in $LIST_OF_HOSTS; do
	ssh i10pc$host "bash -l -c 'exclusive sleep 1'" > /dev/null
	if [ $? -ne 0 ]; then
	    success=0
            echo "i10pc$host reserved, sleeping..."
            sleep 600
            break
    	fi


	result=$(ssh i10pc$host "LC_ALL=en_US.UTF-8 S_TIME_FORMAT=ISO mpstat 2 1" | awk '$12 ~ /[0-9.]+/ { print $12 }' | head -n 1)
	if (( $(echo "(100.0 - $result) > 1.0" | bc) )); then
	    success=0
            echo "i10pc$host under load, sleeping..."
            sleep 600
            break
        fi
    done

    if [ $success -eq 1 ]; then 
	break
    fi
done

if [ -n $1 ]; then
	./notifier.py "Machine i10pc$1 is ready to test!" group
else
	./notifier.py "Machines $LIST_OF_HOSTS are all ready to test!" group
fi

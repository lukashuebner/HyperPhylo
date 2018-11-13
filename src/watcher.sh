#!/bin/bash

LIST_OF_HOSTS="127 128 129 132 134"

success=1
while true; do
    for host in $LIST_OF_HOSTS; do
        result=$(ssh i10pc$host "mpstat 2 1" | awk '$12 ~ /[0-9.]+/ { print 100 - $12 }' | head -n 1)
        if [ $result -gt 2 ]; then
	    success=0
            echo "i10pc$host busy, sleeping..."
            sleep 600
            break
        fi
    done

    if [ $success -eq 1 ]; then 
	break
    fi
done

./notifier.py "Machines are all ready to test!"

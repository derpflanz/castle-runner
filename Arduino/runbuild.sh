#!/bin/bash

upload() {
	./build.sh $1
	
	echo "Opening serial port, press Ctrl-C to go back to build."
#	python showserial.py
}

trap "upload $1" SIGQUIT

if [[ "$1" == "" ]]; then
	echo "Need a tree to watch."
	exit 1
fi

while true; do
	echo "Waiting for files to change in $1... Press Ctrl-\ to upload firmware. Ctrl-C to stop."
	CHANGED=`inotifywait -q -e modify,create -r $1/ --format %f`

	if [[ "$CHANGED" == *.cpp || "$CHANGED" == *.h || "$CHANGED" == *.ino || "$CHANGED" == *.S ]]; then
		echo "$CHANGED changed, rebuild $1"
		./build.sh $1 build
	fi
done
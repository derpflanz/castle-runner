
#!/bin/bash

red() {
    echo -e "\e[31m$1\e[39m"
}

green() {
    echo -e "\e[32m$1\e[39m"
}

if [ "$1" == "" ]; then
    red "Usage: $0 <project>";
    exit;
fi

if [ ! -d "$1" ]; then
    red "$1 is not a dir"
    exit;
fi

if [ ! -f "$1/build.conf" ]; then
    red "build.conf not found in $1, don't know how to build this"
    exit;
else
    . $1/build.conf
fi

green "Going to build project '$1' for board '$BOARD'"
echo

CMD="arduino-cli compile --fqbn $BOARD $1"
echo $CMD
$CMD

if [ $? -eq 1 ]; then
    red "Compilation failed."
    exit
fi

if [ "$2" == "build" ]; then
    green "Only build. Done."
    exit
fi


LINE=`arduino-cli board list | grep $BOARD`

if [ "$LINE" = "" ]; then
    red "Board $BOARD not connected. Could not upload."
    exit;
fi

PORT=`echo $LINE | awk '{ print $1; }'`
green "Found $BOARD on port $PORT"
echo

CMD="arduino-cli upload $1 --fqbn $BOARD --port $PORT"
echo $CMD
$CMD

echo
green "Done."

#python showserial.py

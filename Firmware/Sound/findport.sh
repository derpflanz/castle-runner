#!/bin/bash

# --- Usage and Input Validation ---

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <end_number> <avr_chip_type>"
    echo "Example: $0 5 m328p"
    echo "This scans /dev/ttyACM0 to /dev/ttyACM5 for an ATmega328P chip."
    exit 1
fi

END_NUM=$1
AVR_CHIP=$2
PROGRAMMER_TYPE="stk500v2"

echo "--- AVR Chip Scanner ---"
echo "Target Chip: ${AVR_CHIP}"
echo "Scanning /dev/ttyACM0 through /dev/ttyACM${END_NUM}..."
echo "--------------------------"

# --- Scanning Loop ---

FOUND_PORT=""

for i in $(seq 0 $END_NUM); do
    PORT="/dev/ttyACM${i}"

    # 1. Check if the device file exists
    if [ ! -c "$PORT" ]; then
        # echo "Skipping ${PORT}: Device file not found."
        continue
    fi

    echo -n "Checking ${PORT}..."

    # 2. Attempt to read the device signature using avrdude
    # -c programmer: avr109/arduino is common for bootloaders
    # -p part: The target AVR chip type (e.g., m328p)
    # -P port: The serial port
    # -n: Don't write anything
    # -q -q: Suppress almost all output
    # 2>&1: Redirect stderr (where avrdude prints errors) to stdout for suppression
    # || true: Prevents the script from exiting if avrdude fails
    
    # We test for success by checking the exit code of avrdude
    avrdude -c "${PROGRAMMER_TYPE}" -p "${AVR_CHIP}" -P "${PORT}" -n -q -q 2>/dev/null
    
    # Check the exit status of the previous command
    if [ $? -eq 0 ]; then
        echo " ${AVR_CHIP} FOUND!"
        FOUND_PORT="$PORT"
        break # Exit the loop once the chip is found
    else
        echo " Failed. (Chip/Programmer not found or signature mismatch)"
    fi
done

# --- Final Report ---

echo "--------------------------"
if [ -n "$FOUND_PORT" ]; then
    echo "✅ Success: ${AVR_CHIP} found on ${FOUND_PORT}"
else
    echo "❌ Failure: ${AVR_CHIP} not found on any port in the range."
fi

echo "--------------------------"
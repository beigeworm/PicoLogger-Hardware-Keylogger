#!/bin/bash

BAUD_RATE=115200
VID="239a"
PID="cafe"

echo "Searching for Pico with VID:$VID PID:$PID..."

# Step 1: Confirm device is present via lsusb
if ! lsusb | grep -i "${VID}:${PID}" > /dev/null; then
    echo "Pico not found in lsusb!"
    exit 1
fi

echo "Pico detected via lsusb."

# Step 2: Try to locate matching /dev/ttyACM* device (most recently created)
DEVICE=$(ls -t /dev/ttyACM* 2>/dev/null | head -n 1)

if [ -z "$DEVICE" ]; then
    echo "No /dev/ttyACM* device found. Pico may not have a serial interface enabled."
    exit 1
fi

echo "Using serial device: $DEVICE"

# Step 3: Configure serial port
stty -F "$DEVICE" $BAUD_RATE cs8 -cstopb -parenb -icanon -echo

# Step 4: Main loop
while true; do
    if read -t 0.5 -r line < "$DEVICE"; then
        command=$(echo "$line" | xargs) # Trim whitespace
        if [ -n "$command" ]; then
            echo "Received: $command"

            # Execute command and capture output
            output=$(bash -c "$command" 2>&1)
            response="${output:-[Command executed successfully, no output]}"
            
            echo "Sending response..."
            echo "$response" > "$DEVICE"
        fi
    fi
    sleep 0.2
done

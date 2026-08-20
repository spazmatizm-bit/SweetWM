#!/bin/bash

echo "=== Rebuilding SweetWM and testing in Xephyr ==="

cd /home/spazmatizm/Documents/sweetwm/

echo "Cleaning..."
make clean

echo "Building..."
make

if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo "Build successful!"

# Проверяем, запущен ли Xephyr
pkill -x Xephyr 2>/dev/null

echo "Starting Xephyr..."
Xephyr :1 -screen 1280x720 -ac -br -reset -terminate &
sleep 2

echo "Starting SweetWM..."
DISPLAY=:1 ./sweetwm

# После завершения WM
pkill -x Xephyr 2>/dev/null

echo "Done."

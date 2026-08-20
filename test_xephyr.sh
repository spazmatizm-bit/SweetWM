#!/bin/bash

# Скрипт для тестирования SweetWM в Xephyr

echo "=== SweetWM Test in Xephyr ==="

# Проверяем, запущен ли Xephyr
if pgrep -x "Xephyr" > /dev/null; then
    echo "Xephyr already running, killing..."
    pkill -x Xephyr
    sleep 1
fi

# Запускаем Xephyr
echo "Starting Xephyr on :1..."
Xephyr :1 -screen 1280x720 -ac -br -reset -terminate &
XEPHYR_PID=$!

sleep 2

# Проверяем, что Xephyr запустился
if ! kill -0 $XEPHYR_PID 2>/dev/null; then
    echo "Failed to start Xephyr"
    exit 1
fi

echo "Xephyr started (PID: $XEPHYR_PID)"

# Запускаем SweetWM
echo "Starting SweetWM..."
DISPLAY=:1 ./sweetwm &
SWEET_PID=$!

sleep 1

echo ""
echo "=== SweetWM is running ==="
echo "Display: :1"
echo ""
echo "Hotkeys:"
echo "  Ctrl+Shift+T - xterm"
echo "  Ctrl+Shift+K - kitty"
echo "  Mod4+Q - close window"
echo "  Mod4+Tab - switch window"
echo "  Mod4+M - minimize"
echo "  Mod4+F - maximize"
echo ""
echo "Click 'Menu' button for app launcher"
echo ""
echo "To test, open another terminal and run:"
echo "  DISPLAY=:1 xterm"
echo "  DISPLAY=:1 kitty"
echo "  DISPLAY=:1 firefox"
echo ""
echo "Press Ctrl+C to stop SweetWM and Xephyr"
echo ""

# Ждем завершения
wait $SWEET_PID

echo "SweetWM stopped, killing Xephyr..."
kill $XEPHYR_PID 2>/dev/null
wait $XEPHYR_PID 2>/dev/null

echo "Done."

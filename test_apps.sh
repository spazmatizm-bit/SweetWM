#!/bin/bash

# Скрипт для запуска тестовых приложений в Xephyr

DISPLAY=:1

echo "=== Opening test applications on :1 ==="

# Запускаем несколько приложений
$xterm &
$xclock &
$xcalc &

echo "Opened: xterm, xclock, xcalc"
echo "To open more: DISPLAY=:1 firefox"
echo "To open: DISPLAY=:1 kitty"


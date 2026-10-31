@echo off
title HobTools uninstaller
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0HobTools\Install.ps1" -Uninstall
pause

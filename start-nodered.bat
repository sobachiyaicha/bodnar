@echo off
chcp 65001 >nul
cd /d "%~dp0"
set "PATH=%~dp0..\tools\node-v26.11.1-win-x64;%PATH%"
node "%~dp0..\tools\nodered\node_modules\node-red\red.js" --userDir "%~dp0nodered\runtime" "%~dp0nodered\flow.json"
pause

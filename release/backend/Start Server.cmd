@echo off
title HITMAN PS5 backend
pushd "%~dp0"
node\node.exe start.mjs %*
pause

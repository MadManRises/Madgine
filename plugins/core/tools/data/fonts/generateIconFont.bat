@echo off

call fantasticon icons -o . -t ttf -g json --normalize true
node generateFontHeader.js icons.json ../../src/imguiicons.h

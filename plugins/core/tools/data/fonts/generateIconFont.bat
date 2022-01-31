@echo off

call fantasticon icons -o . -t ttf -g json
node generateFontHeader.js icons.json ../../src/imguiicons.h

set out=%2

IF [%2]==[] set out=%1

powershell -Command "(gc %1.cfg) -creplace 'OISHandler', 'EmscriptenInput' | Out-File -encoding ASCII %out%.cfg"

powershell -Command "(gc components_%1.cpp) -creplace 'OISHandler', 'EmscriptenInput' -creplace 'OISInputHandler', 'EmscriptenInputHandler' -creplace 'oislib', 'emscripteninputlib' -creplace 'oisinputhandler', 'emscripteninputhandler' | Out-File -encoding ASCII components_%out%.cpp"

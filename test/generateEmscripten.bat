"%1/bin/MadgineLauncher.exe" -t -lp OpenGL.cfg -ep OpenGL_emscripten.cfg

powershell -Command "(gc OpenGL_emscripten.cfg) -creplace 'OISHandler', 'EmscriptenInput' | Out-File -encoding ASCII OpenGL_emscripten.cfg"

powershell -Command "(gc components_OpenGL_emscripten.cpp) -creplace 'OISHandler', 'EmscriptenInput' -creplace 'OISInputHandler', 'EmscriptenInputHandler' -creplace 'oislib', 'emscripteninputlib' -creplace 'oisinputhandler', 'emscripteninputhandler' | Out-File -encoding ASCII components_OpenGL_emscripten.cpp"
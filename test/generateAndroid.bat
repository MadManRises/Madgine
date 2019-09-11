"%1/bin/MadgineLauncher.exe" -t -npc -lp OpenGL.cfg -ep OpenGL_clang-android.cfg

powershell -Command "(gc OpenGL_clang-android.cfg) -creplace 'OISHandler', 'AndroidInput' | Out-File -encoding ASCII OpenGL_clang-android.cfg"

powershell -Command "(gc components_OpenGL_clang-android.cpp) -creplace 'OISHandler', 'AndroidInput' -creplace 'OISInputHandler', 'AndroidInputHandler' -creplace 'oislib', 'androidinputlib' -creplace 'oisinputhandler', 'androidinputhandler' | Out-File -encoding ASCII components_OpenGL_clang-android.cpp"

set out=%2

IF [%2]==[] set out=%1

powershell -Command "(gc %1.cfg) -creplace 'OISHandler', 'AndroidInput' | Out-File -encoding ASCII %out%.cfg"

powershell -Command "(gc components_%1.cpp) -creplace 'OISHandler', 'AndroidInput' -creplace 'OISInputHandler', 'AndroidInputHandler' -creplace 'oislib', 'androidinputlib' -creplace 'oisinputhandler', 'androidinputhandler' | Out-File -encoding ASCII components_%out%.cpp"

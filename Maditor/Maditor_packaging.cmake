if (MSVC)

	file(DOWNLOAD https://aka.ms/vs/15/release/vs_buildtools.exe ${CMAKE_BINARY_DIR}/extern/vs15.exe)

	install(PROGRAMS ${CMAKE_BINARY_DIR}/extern/vs15.exe DESTINATION tmp COMPONENT vs15)

	list(APPEND CPACK_NSIS_EXTRA_INSTALL_COMMANDS " 
        ExecWait '$INSTDIR\\\\tmp\\\\vs15.exe --wait --norestart --add Microsoft.VisualStudio.Workload.MSBuildTools --add Microsoft.VisualStudio.Component.VC.Tools.x86.x64 --add Microsoft.VisualStudio.Component.Windows10SDK.17134 --add Microsoft.VisualStudio.Component.VC.CoreBuildTools'
    ")

	list(APPEND CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS " 
        ExecWait '$INSTDIR\\\\tmp\\\\vs15.exe --wait --norestart --remove Microsoft.VisualStudio.Workload.MSBuildTools --remove Microsoft.VisualStudio.Component.VC.Tools.x86.x64 --remove Microsoft.VisualStudio.Component.Windows10SDK.17134 --remove Microsoft.VisualStudio.Component.VC.CoreBuildTools'
    ")

endif (MSVC)
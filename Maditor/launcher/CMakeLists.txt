cmake_minimum_required (VERSION 2.6)


set (BUILD_CLIENT TRUE CACHE BOOL "")


set (sources "")
FolderSources(sources "" "" applicationwrapper.cpp applicationwrapper.h embeddedwrapper.cpp embeddedwrapper.h inputwrapper.cpp inputwrapper.h inspector.cpp inspector.h maditorlauncherlib.cpp maditorlauncherlib.h maditorlauncherforward.h main_embedded.cpp moduleloader.cpp moduleloader.h)
	FolderSources(sources "" util appstats.cpp appstats.h launcherloglistener.cpp launcherloglistener.h util.cpp util.h)	


add_executable(Embedded_Server_Launcher ${sources})
target_link_libraries(Embedded_Server_Launcher PRIVATE Maditor_Shared MadgineServer lualib)

if (CMAKE_COMPILER_IS_GNUCXX)
	target_link_libraries(Embedded_Server_Launcher PRIVATE dl)
endif (CMAKE_COMPILER_IS_GNUCXX)

if (BUILD_CLIENT)
	add_executable(Embedded_Client_Launcher ${sources})
	target_link_libraries(Embedded_Client_Launcher PRIVATE Maditor_Shared OgreMadgine lualib)

	if (CMAKE_COMPILER_IS_GNUCXX)
		target_link_libraries(Embedded_Client_Launcher PRIVATE dl)
	endif (CMAKE_COMPILER_IS_GNUCXX)

	install_to_workspace(Maditor TARGETS Embedded_Client_Launcher)

endif (BUILD_CLIENT)

add_subdirectory(standalone)

install_to_workspace(Maditor TARGETS Embedded_Server_Launcher)

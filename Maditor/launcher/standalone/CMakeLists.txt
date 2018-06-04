

PROJECT(Maditor_Standalone_Launcher)

set (sources "")
FolderSources(sources "" "" standalonewrapper.cpp standalonewrapper.h main_standalone.cpp)
FolderSources(sources "" ".." applicationwrapper.cpp applicationwrapper.h embeddedwrapper.cpp embeddedwrapper.h inputwrapper.cpp inputwrapper.h inspector.cpp inspector.h maditorlauncherlib.cpp maditorlauncherlib.h maditorlauncherforward.h moduleloader.cpp moduleloader.h)
	FolderSources(sources ".." util appstats.cpp appstats.h launcherloglistener.cpp launcherloglistener.h util.cpp util.h)	


add_definitions(-DCONFIG="${CONFIG}")

add_executable(Standalone_Server_Launcher ${sources})
target_link_libraries(Standalone_Server_Launcher PRIVATE Maditor_Shared MadgineServer lualib)

if (CMAKE_COMPILER_IS_GNUCXX)
	target_link_libraries(Standalone_Server_Launcher PRIVATE dl)
endif (CMAKE_COMPILER_IS_GNUCXX)

if (BUILD_CLIENT)
	add_executable(Standalone_Client_Launcher ${sources})
	target_link_libraries(Standalone_Client_Launcher PRIVATE Maditor_Shared OgreMadgine lualib)

	if (CMAKE_COMPILER_IS_GNUCXX)
		target_link_libraries(Standalone_Client_Launcher PRIVATE dl)
	endif (CMAKE_COMPILER_IS_GNUCXX)

endif (BUILD_CLIENT)



set (sources "")
FolderSources(sources "" "" standalonewrapper.cpp standalonewrapper.h main_standalone.cpp)
FolderSources(sources "" ".." applicationwrapper.cpp applicationwrapper.h embeddedwrapper.cpp embeddedwrapper.h inputwrapper.cpp inputwrapper.h inspector.cpp inspector.h maditorlauncherlib.h maditorlauncherforward.h moduleloader.cpp moduleloader.h)
	FolderSources(sources ".." util appstats.cpp appstats.h launcherloglistener.cpp launcherloglistener.h util.cpp util.h)	


add_definitions(-DCONFIG="${CONFIG}")

add_executable(Standalone_Launcher ${sources})
target_link_libraries(Standalone_Launcher PRIVATE Maditor_Shared Client lualib)

if (UNIX)
	target_link_libraries(Standalone_Launcher PRIVATE dl)
endif (UNIX)

cmake_minimum_required (VERSION 2.6)


set (sources "")
FolderSources(sources "" "" applicationwrapper.cpp applicationwrapper.h embeddedwrapper.cpp embeddedwrapper.h inputwrapper.cpp inputwrapper.h inspector.cpp inspector.h maditorlauncherlib.h maditorlauncherforward.h main_embedded.cpp moduleloader.cpp moduleloader.h)
	FolderSources(sources "" util appstats.cpp appstats.h launcherloglistener.cpp launcherloglistener.h util.cpp util.h)	


add_executable(Embedded_Launcher ${sources})
target_link_libraries(Embedded_Launcher PRIVATE Maditor_Shared Client lualib)

add_dependencies(Embedded_Launcher Tools)

if (UNIX)
	target_link_libraries(Embedded_Launcher PRIVATE dl)
endif (UNIX)

add_subdirectory(standalone)


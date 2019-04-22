
include (android)

set(sources "")
FolderSources(sources "" "" launcher.cpp)
if (ANDROID)
	FolderSources(sources "" android androidlauncher.cpp androidlauncher.h)
endif(ANDROID)

add_workspace_application(MadgineLauncher ${sources})

target_link_plugins(MadgineLauncher PUBLIC Client Tools OpenGL)


if (ANDROID)
	target_link_libraries(MadgineLauncher PRIVATE android log)	
endif()

if (ANDROID)
	target_link_plugins(MadgineLauncher PUBLIC AndroidInput)
else()
	target_link_plugins(MadgineLauncher PUBLIC OISHandler)
endif()

install_to_workspace(Madgine TARGETS MadgineLauncher)
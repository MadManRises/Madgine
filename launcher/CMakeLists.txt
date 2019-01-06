
include (android)

set(sources "")
FolderSources(sources "" "" launcher.cpp)
if (ANDROID)
	FolderSources(sources "" android androidlauncher.cpp androidlauncher.h)
endif(ANDROID)

add_workspace_executable(MadgineLauncher ${sources})

target_link_plugins(MadgineLauncher PUBLIC Client Tools OpenGL)

if (NOT ANDROID)
	target_link_plugins(MadgineLauncher PUBLIC OISHandler)
endif()

install_to_workspace(Madgine TARGETS MadgineLauncher)

include (android)

set(sources "")
FolderSources(sources "" "" launcher.cpp)
	FolderSources(sources "" android androidlauncher.cpp androidlauncher.h)

add_executable(MadgineLauncher ${sources})

target_link_plugins(MadgineLauncher PUBLIC Client Tools OpenGL)

if (NOT ANDROID)
	target_link_plugins(MadgineLauncher PUBLIC OISHandler)
endif()

install_to_workspace(Madgine TARGETS MadgineLauncher)
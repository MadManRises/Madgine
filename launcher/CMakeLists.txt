

set(sources "")
FolderSources(sources "" "" launcher.cpp)
if (ANDROID)
	FolderSources(sources "" android androidlauncher.cpp androidlauncher.h)
endif(ANDROID)

add_workspace_application(MadgineLauncher ${sources})

target_link_plugins(MadgineLauncher Client Tools OpenGL Base)

if (ANDROID)
	target_link_libraries(MadgineLauncher PRIVATE android log)	
endif()

if (ANDROID)
	target_link_plugins(MadgineLauncher AndroidInput)
elseif (EMSCRIPTEN)
	target_link_plugins(MadgineLauncher EmscriptenInput)
else()
	target_link_plugins(MadgineLauncher OISHandler)
endif()

if (PLUGINS_BUILD_SDLINPUT)
	target_link_plugins(MadgineLauncher SDLInput)
endif()


generate_binary_info(MadgineLauncher)

install_to_workspace(Madgine TARGETS MadgineLauncher)
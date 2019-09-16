

set(sources "")
FolderSources(sources "" "" launcher.cpp)
if (ANDROID)
	FolderSources(sources "" android androidlauncher.cpp androidlauncher.h)
endif(ANDROID)

add_workspace_application(MadgineLauncher ${sources})

target_link_plugins(MadgineLauncher Base)

if (EMSCRIPTEN)
	_target_link_libraries(MadgineLauncher PRIVATE "--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/emscripten/template.html")
elseif (ANDROID)
	target_link_libraries(MadgineLauncher PRIVATE android)	
	target_link_plugins(MadgineLauncher AndroidInput)
endif()

target_depend_on_all_plugins(MadgineLauncher)

target_link_libraries(MadgineLauncher PRIVATE freetype)
generate_binary_info(MadgineLauncher)

install_to_workspace(Madgine TARGETS MadgineLauncher)

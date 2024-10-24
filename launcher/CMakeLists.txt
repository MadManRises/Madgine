
set(sources "")
FolderSources(sources "" "" launcher.cpp main.cpp terminate_handler.cpp)
if (EMSCRIPTEN)
	FolderSources(sources "" emscripten emscripten_main.cpp)
endif(EMSCRIPTEN)
if (ANDROID)
	FolderSources(sources "" android android_main.cpp androidlauncher.cpp androidlauncher.h)
endif(ANDROID)
if (OSX)
    FolderSources(sources "" osx osx_main.mm)
endif (OSX)
if (IOS)
	FolderSources(sources "" ios ios_main.mm)
endif (IOS)

add_workspace_application(MadgineLauncher ${sources})

set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT MadgineLauncher)

if (MADGINE_AS_SDK)
	MESSAGE(${MADGINE_BINARY_NAME})
	set_target_properties(MadgineLauncher PROPERTIES OUTPUT_NAME ${MADGINE_BINARY_NAME})
elseif (NOT MODULES_ENABLE_PLUGINS)
	set_target_properties(MadgineLauncher PROPERTIES OUTPUT_NAME MadgineLauncher_${PLUGIN_DEFINITION_NAME})
endif()

target_link_plugins(MadgineLauncher App Client UI Root)

if (EMSCRIPTEN)
	_target_link_libraries(MadgineLauncher PRIVATE "--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/emscripten/template.html")
elseif (ANDROID)
	target_link_libraries(MadgineLauncher PRIVATE android)	
elseif (OSX)
    target_link_libraries(MadgineLauncher PRIVATE "-framework Cocoa")
elseif (IOS)
    target_link_libraries(MadgineLauncher PRIVATE "-framework UIKit")
endif()



target_depend_on_all_plugins(MadgineLauncher)

generate_binary_info(MadgineLauncher)

install_to_workspace(Madgine TARGETS MadgineLauncher)


set(MADGINE_LAUNCHER_SPLASH_IMAGE "" CACHE FILEPATH "Path to an image file for the applications splash screen")
set(MADGINE_LAUNCHER_WINDOW_TITLE "" CACHE STRING "Title of the Main window of the game")
set(MADGINE_LAUNCHER_ICON "${CMAKE_CURRENT_SOURCE_DIR}/madgineIcon.ico" CACHE FILEPATH "Path to an image file for the applications tray icon")

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
if (WINDOWS)
	FolderSources(sources "" windows windows_main.cpp)
endif (WINDOWS)

add_workspace_application(MadgineLauncher ${sources})

set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT MadgineLauncher)

if (MADGINE_AS_SDK)
	MESSAGE("Using binary name: ${MADGINE_BINARY_NAME}")
	set_target_properties(MadgineLauncher PROPERTIES OUTPUT_NAME ${MADGINE_BINARY_NAME})
elseif (MADGINE_CONFIGURATION)
	get_filename_component(name ${MADGINE_CONFIGURATION} NAME)
	set_target_properties(MadgineLauncher PROPERTIES OUTPUT_NAME MadgineLauncher_${name})
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

if (WINDOWS)
	configure_file(windows/resources.in.rc resources.rc)
	configure_file(windows/resources.in.h resources.h)
	target_sources(MadgineLauncher PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/resources.rc)
endif ()

configure_file(launcherconfig.in.h launcherconfig.h)
target_include_directories(MadgineLauncher PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)

target_depend_on_all_plugins(MadgineLauncher)

generate_binary_info(MadgineLauncher)

install_to_workspace(Madgine TARGETS MadgineLauncher)


file(GLOB lists "${MADGINE_CONFIGURATION}/*.list")
if (lists)
	collect_data_lists("${lists}")
endif()

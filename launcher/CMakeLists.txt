

set(sources "")
FolderSources(sources "" "" launcher.cpp)
if (EMSCRIPTEN)
	FolderSources(sources "" emscripten emscripten_main.cpp)
endif(EMSCRIPTEN)
if (ANDROID)
	FolderSources(sources "" android android_main.cpp androidlauncher.cpp androidlauncher.h)
endif(ANDROID)

add_workspace_application(MadgineLauncher ${sources})

if (MADGINE_AS_SDK)
	MESSAGE(${MADGINE_BINARY_NAME})
	set_target_properties(MadgineLauncher PROPERTIES OUTPUT_NAME ${MADGINE_BINARY_NAME})
endif()

target_link_plugins(MadgineLauncher Base Client)
target_link_libraries(MadgineLauncher PRIVATE Xml)

if (EMSCRIPTEN)
	_target_link_libraries(MadgineLauncher PRIVATE "--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/emscripten/template.html")
elseif (ANDROID)
	target_link_libraries(MadgineLauncher PRIVATE android)	
endif()

target_depend_on_all_plugins(MadgineLauncher)

generate_binary_info(MadgineLauncher)

install_to_workspace(Madgine TARGETS MadgineLauncher)

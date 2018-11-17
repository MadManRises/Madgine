cmake_minimum_required (VERSION 2.6)

cmake_policy(SET CMP0049 OLD)
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0022 NEW)

Project(Tools)

set(sources "")
FolderSources(sources "" src tool.h toolbase.cpp toolbase.h toolscollector.h)
	FolderSources(sources src imgui imgui.cpp imgui.h imgui_demo.cpp imgui_draw.cpp imgui_internal.h)
		FolderSources(sources imgui examples imgui_impl_opengl3.cpp imgui_impl_opengl3.h)
	FolderSources(sources src inspector inspector.cpp inspector.h)
	FolderSources(sources src memory memoryviewer.cpp memoryviewer.h)
	FolderSources(sources src metrics metrics.cpp metrics.h)
	FolderSources(sources src pluginmanager pluginmanager.cpp pluginmanager.h)
	FolderSources(sources src profiler profiler.cpp profiler.h)
	FolderSources(sources src renderer imguiaddons.cpp imguiaddons.h imguidemo.cpp imguidemo.h imguimanager.cpp imguimanager.h imguiroot.cpp imguiroot.h)
	if (NOT STATIC_BUILD OR PLUGINSELECTION_Renderer_OpenGL)
		FolderSources(sources renderer opengl openglimguimanager.cpp openglimguimanager.h)
	endif()
	FolderSources(sources src tinyxml tinyxml2.cpp tinyxml2.h)
	

add_plugin(Tools Madgine Utility ${sources})
add_precompiled_header(Tools src/toolslib.h FORCEINCLUDE)


target_link_plugins(Tools PUBLIC OpenGL)


target_link_libraries(Tools PUBLIC Client)

target_compile_definitions(Tools PRIVATE -DBINARY_FILENAME="$<TARGET_FILE_NAME:Tools>")

add_custom_command(TARGET Tools POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/Tools/layouts
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/src/inspector/layouts ${CMAKE_BINARY_DIR}/Tools/layouts
)

install(DIRECTORY ${CMAKE_BINARY_DIR}/Tools/layouts DESTINATION Tools COMPONENT Tools)

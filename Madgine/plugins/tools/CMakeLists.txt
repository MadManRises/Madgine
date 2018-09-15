cmake_minimum_required (VERSION 2.6)

cmake_policy(SET CMP0049 OLD)
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0022 NEW)

Project(Tools)

include_directories(${OgreMadgine_SOURCE_DIR}/src)

set(sources "")
FolderSources(sources "" "" tool.h toolbase.cpp toolbase.h toolscollector.h)
	FolderSources(sources "" imgui imgui.cpp imgui.h imgui_draw.cpp imgui_internal.h)
	FolderSources(sources "" inspector inspector.cpp inspector.h)
	FolderSources(sources "" memory memoryviewer.cpp memoryviewer.h)
	FolderSources(sources "" renderer imguiroot.cpp imguiroot.h)
		FolderSources(sources renderer ogre ImguiManager.cpp ImguiManager.h ImguiRenderable.cpp ImguiRenderable.h)
	FolderSources(sources "" tinyxml tinyxml2.cpp tinyxml2.h)
	

add_plugin(Tools Madgine Utility ${sources})
add_precompiled_header(Tools toolslib.h FORCEINCLUDE)

target_link_libraries(Tools PUBLIC Client PRIVATE OgreMadgine DbgHelp)

install_to_workspace(Madgine TARGETS Tools)

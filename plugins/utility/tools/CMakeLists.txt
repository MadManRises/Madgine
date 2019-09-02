cmake_minimum_required (VERSION 2.6)

Project(Tools)

set(sources "")
FolderSources(sources "" src toolbase.cpp toolbase.h toolscollector.cpp toolscollector.h)
	FolderSources(sources src guieditor guieditor.cpp guieditor.h widgetsettings.cpp widgetsettings.h)
	FolderSources(sources src inspector functiontool.cpp functiontool.h inspector.cpp inspector.h inspectorlayout.cpp inspectorlayout.h layoutloader.cpp layoutloader.h)
	FolderSources(sources src memory memoryviewer.cpp memoryviewer.h)
	FolderSources(sources src metrics metrics.cpp metrics.h)
	FolderSources(sources src pluginmanager pluginmanager.cpp pluginmanager.h)
	FolderSources(sources src profiler profiler.cpp profiler.h)
	FolderSources(sources src renderer imguidemo.cpp imguidemo.h imguimanager.cpp imguimanager.h imguiroot.cpp imguiroot.h)
	if (MODULES_ENABLE_PLUGINS OR PLUGINSELECTION_Renderer_OpenGL)
		FolderSources(sources renderer opengl openglimguimanager.cpp openglimguimanager.h)
	endif()
	FolderSources(sources src sceneeditor sceneeditor.cpp sceneeditor.h sceneview.cpp sceneview.h)
		FolderSources(sources sceneeditor opengl openglgridpass.cpp openglgridpass.h)
	FolderSources(sources src testtool testtool.cpp testtool.h)
	FolderSources(sources src tinyxml tinyxml2.cpp tinyxml2.h)
	

add_plugin(Tools Madgine Utility ${sources} PRECOMPILED_HEADER src/toolslib.h)

target_link_libraries(Tools PUBLIC Apis)

target_link_plugins(Tools OpenGL Client Base)


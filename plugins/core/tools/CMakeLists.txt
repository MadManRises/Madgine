cmake_minimum_required (VERSION 2.6)

Project(Tools)

set(sources "")
FolderSources(sources "" src toolbase.cpp toolbase.h toolscollector.cpp toolscollector.h)
	FolderSources(sources src inspector functiontool.cpp functiontool.h inspector.cpp inspector.h inspectorlayout.cpp inspectorlayout.h layoutloader.cpp layoutloader.h)
	FolderSources(sources src memory memoryviewer.cpp memoryviewer.h)
	FolderSources(sources src metrics metrics.cpp metrics.h)
	FolderSources(sources src pluginmanager pluginmanager.cpp pluginmanager.h)
	FolderSources(sources src profiler profiler.cpp profiler.h)
	FolderSources(sources src project projectmanager.cpp projectmanager.h)
	FolderSources(sources src renderer imguidemo.cpp imguidemo.h imroot.cpp imroot.h)
	FolderSources(sources src testtool testtool.cpp testtool.h)
	FolderSources(sources src tinyxml tinyxml2.cpp tinyxml2.h)
	

add_plugin(Tools Madgine Tools ${sources} PRECOMPILED_HEADER src/toolslib.h)

target_link_libraries(Tools PUBLIC ImGui Im3D Base)

target_link_plugins(Tools ImageLoader)


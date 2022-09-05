
PROJECT(Resources)

set(sources "")
FolderSources(sources "" src)
	FolderSources(sources src Madgine)
		FolderSources(sources Madgine resources resourcebase.cpp resourcebase.h resourceloaderbase.cpp resourceloaderbase.h resourceloadercollector.cpp resourceloadercollector.h resourceloader.cpp resourceloader.h resourcemanager.cpp resourcemanager.h)

add_plugin(Resources Madgine Core ${sources})

target_link_plugins(Resources Root)

set(sources "")
FolderSources(sources "" tools)
	FolderSources(sources tools Madgine_Tools resourcestoolconfig.cpp resourcestoolconfig.h resourcestoolsforward.h resourcestoolslib.h)


add_tools(Resources ${sources} PRECOMPILED_HEADER tools/Madgine_Tools/resourcestoolslib.h)

target_link_plugins(ResourcesTools Tools)

add_subdirectory(loaders)
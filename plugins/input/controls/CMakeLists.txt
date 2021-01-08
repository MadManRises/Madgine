
Project(Controls)

set(sources "")
FolderSources(sources "" src)
	FolderSources(sources src Madgine controlsforward.h controlslib.h)
		FolderSources(sources Madgine controls controlsmanager.cpp)

add_plugin(Controls Madgine Input ${sources} PRECOMPILED_HEADER src/Madgine/controlslib.h)

target_link_plugins(Controls Base)


set(sources "")
FolderSources(sources "" tools)
	FolderSources(sources tools Madgine_Tools controlstoolslib.h controlstoolsforward.h)
		FolderSources(sources Madgine_Tools controls controlsview.cpp controlsview.h)

add_tools(Controls ${sources} PRECOMPILED_HEADER tools/Madgine_Tools/controlstoolslib.h SOURCE_ROOT tools)

target_link_plugins(ControlsTools Tools)

Project(ClientControls)

set(sources "")
FolderSources(sources "" client)
	FolderSources(sources client Madgine clientcontrolsforward.h clientcontrolslib.h)
		FolderSources(sources Madgine controls clientcontrolsmanager.cpp)

add_plugin(ClientControls Madgine Input ${sources} SOURCE_ROOT client PRECOMPILED_HEADER client/Madgine/clientcontrolslib.h)

target_link_plugins(ClientControls Client Controls)



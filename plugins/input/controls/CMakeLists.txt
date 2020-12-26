
Project(Controls)

set(sources "")
FolderSources(sources "" src)
	FolderSources(sources src Madgine controlsforward.h controlslib.h)
		FolderSources(sources Madgine controls controlsmanager.cpp)

add_plugin(Controls Madgine Input ${sources} PRECOMPILED_HEADER src/Madgine/controlslib.h)

target_link_plugins(Controls Base)


Project(ClientControls)

set(sources "")
FolderSources(sources "" client)
	FolderSources(sources client Madgine clientcontrolsforward.h clientcontrolslib.h)
		FolderSources(sources Madgine controls clientcontrolsmanager.cpp)

add_plugin(ClientControls Madgine Input ${sources} SOURCE_ROOT client PRECOMPILED_HEADER client/Madgine/clientcontrolslib.h)

target_link_plugins(ClientControls Client Controls)

#set(sources "")
#FolderSources(sources "" renderer_tools)
#	FolderSources(sources renderer_tools Madgine_Tools)
#		FolderSources(sources Madgine_Tools sceneeditor gridpass.cpp gridpass.h sceneeditor.cpp sceneeditor.h sceneview.cpp sceneview.h)

#add_tools(SceneRenderer ${sources} PRECOMPILED_HEADER renderer_tools/Madgine_Tools/scenerenderertoolslib.h SOURCE_ROOT renderer_tools)

#target_link_plugins(SceneRendererTools ClientTools ProgramLoader)
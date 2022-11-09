Project(Scene)

set(sources "")
FolderSources(sources "" src)
	FolderSources(sources src Madgine sceneforward.h scenelib.h)
		FolderSources(sources Madgine scene scenemanager.cpp scenemanager.h scenecomponentcollector.cpp scenecomponentcollector.h scenecomponentbase.cpp scenecomponentbase.h)
			FolderSources(sources scene entity entitycomponentbase.cpp entitycomponentbase.h entitycomponentcollector.cpp entitycomponentcollector.h entitycomponentptr.cpp entityptr.cpp entityptr.h entity.cpp entity.h entitycomponent.h entitycomponentlistbase.cpp entitycomponentlist.cpp entitycomponenthandle.cpp entitycomponenthandle.h masks.h)
				FolderSources(sources entity components animation.cpp animation.h material.cpp material.h mesh.cpp mesh.h pointlight.cpp pointlight.h skeleton.cpp skeleton.h transform.cpp transform.h)

add_plugin(Scene Madgine Core ${sources} PRECOMPILED_HEADER src/Madgine/scenelib.h)

target_link_plugins(Scene MeshLoader AnimationLoader App)

project(SceneNodes)
set(sources "")
FolderSources(sources "" nodes)
	FolderSources(sources nodes Madgine scenenodesforward.h scenenodeslib.h)
		FolderSources(sources Madgine scene)
			FolderSources(sources scene nodes transformaccessornode.cpp transformaccessornode.h)

add_plugin(SceneNodes Madgine Core ${sources} PRECOMPILED_HEADER nodes/Madgine/scenenodeslib.h SOURCE_ROOT nodes)

target_link_plugins(SceneNodes Scene NodeGraph)


Project(SceneRenderer)

set(sources "")
FolderSources(sources "" renderer)
	FolderSources(sources renderer Madgine scenerendererforward.h scenerendererlib.h)
		FolderSources(sources Madgine render pointshadowrenderpass.cpp pointshadowrenderpass.h scenemainwindowcomponent.cpp scenemainwindowcomponent.h scenerenderdata.cpp scenerenderdata.h scenerenderpass.cpp scenerenderpass.h shadowrenderpass.cpp shadowrenderpass.h)
			FolderSources(sources render shaders light.hlsl lightdata.sl pointshadow.GS_hlsl pointshadow.PS_hlsl pointshadow.sl pointshadow.VS_hlsl scene.PS_hlsl scene.sl scene.VS_hlsl)

add_plugin(SceneRenderer Madgine Core ${sources} SOURCE_ROOT renderer PRECOMPILED_HEADER renderer/Madgine/scenerendererlib.h)

target_link_plugins(SceneRenderer Scene Client)

set(sources "")
FolderSources(sources "" renderer_tools)
	FolderSources(sources renderer_tools Madgine_Tools)
		FolderSources(sources Madgine_Tools sceneeditor gridpass.cpp gridpass.h sceneeditor.cpp sceneeditor.h sceneview.cpp sceneview.h)
			FolderSources(sources sceneeditor shaders grid.PS_hlsl grid.sl grid.VS_hlsl)

add_tools(SceneRenderer ${sources} PRECOMPILED_HEADER renderer_tools/Madgine_Tools/scenerenderertoolslib.h SOURCE_ROOT renderer_tools)

target_link_plugins(SceneRendererTools ClientTools PipelineLoader)

target_link_libraries(SceneRendererTools PRIVATE MemorySerialize FilesystemSerialize)

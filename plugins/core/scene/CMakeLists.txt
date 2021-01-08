cmake_minimum_required (VERSION 2.6)

Project(Scene)

set(sources "")
FolderSources(sources "" src)
	FolderSources(sources src Madgine sceneforward.h scenelib.h)
		FolderSources(sources Madgine scene scenemanager.cpp scenemanager.h scenecomponentcollector.cpp scenecomponentcollector.h scenecomponentbase.cpp scenecomponentbase.h)
			FolderSources(sources scene entity entitycomponentbase.cpp entitycomponentbase.h entitycomponentcollector.cpp entitycomponentcollector.h entitycomponentptr.cpp componentexception.h entityptr.cpp entityptr.h entity.cpp entity.h entitycomponent.h entitycomponentlist.cpp entitycomponenthandle.cpp entitycomponenthandle.h masks.h)
				FolderSources(sources entity components animation.cpp animation.h mesh.cpp mesh.h skeleton.cpp skeleton.h transform.cpp transform.h)

add_plugin(Scene Madgine Core ${sources} PRECOMPILED_HEADER src/Madgine/scenelib.h)

target_link_plugins(Scene MeshLoader SkeletonLoader AnimationLoader Base)

Project(SceneRenderer)

set(sources "")
FolderSources(sources "" renderer)
	FolderSources(sources renderer Madgine scenerendererforward.h scenerendererlib.h)
		FolderSources(sources Madgine render scenerenderpass.cpp scenerenderpass.h)

add_plugin(SceneRenderer Madgine Core ${sources} SOURCE_ROOT renderer PRECOMPILED_HEADER renderer/Madgine/scenerendererlib.h)

target_link_plugins(SceneRenderer Scene Client ProgramLoader)

set(sources "")
FolderSources(sources "" renderer_tools)
	FolderSources(sources renderer_tools Madgine_Tools)
		FolderSources(sources Madgine_Tools sceneeditor gridpass.cpp gridpass.h sceneeditor.cpp sceneeditor.h sceneview.cpp sceneview.h)

add_tools(SceneRenderer ${sources} PRECOMPILED_HEADER renderer_tools/Madgine_Tools/scenerenderertoolslib.h SOURCE_ROOT renderer_tools)

target_link_plugins(SceneRendererTools ClientTools ProgramLoader)

target_link_libraries(SceneRendererTools PRIVATE Memory Xml Filesystem)

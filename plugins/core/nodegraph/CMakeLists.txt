cmake_minimum_required (VERSION 2.6)

Project(NodeGraph)

set(sources "")
FolderSources(sources "" src)
	FolderSources(sources src Madgine nodegraphforward.h nodegraphlib.h)
		FolderSources(sources Madgine nodegraph codegenerator.cpp codegenerator.h nodegraph.cpp nodegraph.h nodegraphloader.cpp nodegraphloader.h nodebase.cpp nodebase.h nodecollector.cpp nodecollector.h nodeinterpreter.cpp nodeinterpreter.h)
			FolderSources(sources nodegraph nodes functionnode.cpp functionnode.h pumpnode.cpp pumpnode.h testnode.cpp testnode.h variablenode.cpp variablenode.h)
				FolderSources(sources nodes arithmetical additionnode.cpp additionnode.h multiplicationnode.cpp multiplicationnode.h vector3to4node.cpp vector3to4node.h)
				FolderSources(sources nodes util)
			
add_plugin(NodeGraph Madgine Core ${sources} PRECOMPILED_HEADER src/Madgine/nodegraphlib.h)

target_link_plugins(NodeGraph Resources)
target_link_libraries(NodeGraph PUBLIC FilesystemSerialize CodeGen)


Project(SceneRenderer)

set(sources "")
FolderSources(sources "" tools)
	FolderSources(sources tools Madgine_Tools nodegraphtoolsforward.h nodegraphtoolslib.h)
		FolderSources(sources Madgine_Tools nodegraph nodegrapheditor.cpp nodegrapheditor.h)

add_tools(NodeGraph ${sources} SOURCE_ROOT tools PRECOMPILED_HEADER tools/Madgine_Tools/nodegraphtoolslib.h)

target_link_plugins(NodeGraphTools Tools)
target_link_libraries(NodeGraphTools PUBLIC imgui_node_editor)
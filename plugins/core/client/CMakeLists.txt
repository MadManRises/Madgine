Project(Client)     

set(sources "")
FolderSources(sources "" src)
	FolderSources(sources src Madgine clientforward.h clientlib.h)
		FolderSources(sources Madgine render camera.cpp camera.h rendercontext.cpp rendercontext.h rendertarget.cpp rendertarget.h rendercontextcollector.cpp rendercontextcollector.h)	
			FolderSources(sources render passes bloompass.cpp bloompass.h blurpass.cpp blurpass.h)
			FolderSources(sources render shadinglanguage sl.h sl_include.h slloader.cpp slloader.h)
		FolderSources(sources Madgine window mainwindow.cpp mainwindow.h mainwindowcomponent.cpp mainwindowcomponent.h mainwindowcomponentcollector.cpp mainwindowcomponentcollector.h toolwindow.cpp toolwindow.h)

add_plugin(Client Madgine Core ${sources} PRECOMPILED_HEADER src/Madgine/clientlib.h)

plugin_group(Renderer EXCLUSIVE ATLEAST_ONE)

target_link_plugins(Client PipelineLoader MeshLoader)
target_link_plugin_groups(Client Renderer)

target_link_libraries(Client PUBLIC Modules Render Meta)
target_link_libraries(Client PRIVATE FilesystemSerialize)



project(ClientNodes)
set(sources "")
FolderSources(sources "" nodes)
	FolderSources(sources nodes Madgine clientnodesforward.h clientnodeslib.h)
		FolderSources(sources Madgine client)
			FolderSources(sources client nodes gpubuffernode.cpp gpubuffernode.h framebuffernode.cpp framebuffernode.h meshrenderernode.cpp meshrenderernode.h rasterizernode.cpp rasterizernode.h shadercodegenerator.cpp shadercodegenerator.h)

add_plugin(ClientNodes Madgine Core ${sources} PRECOMPILED_HEADER nodes/Madgine/clientnodeslib.h SOURCE_ROOT nodes)

target_link_plugins(ClientNodes Client NodeGraph PipelineLoader MeshLoader)




set(sources "")
FolderSources(sources "" tools)
	FolderSources(sources tools Madgine_Tools interactivecamera.cpp interactivecamera.h)
		FolderSources(sources Madgine_Tools gamepad gamepadviewer.cpp gamepadviewer.h)
		FolderSources(sources Madgine_Tools im3d im3drenderpass.cpp im3drenderpass.h)
		FolderSources(sources Madgine_Tools imgui clientimroot.cpp clientimroot.h projectmanager.cpp projectmanager.h)

add_tools(Client ${sources} PRECOMPILED_HEADER tools/Madgine_Tools/clienttoolslib.h)

target_link_plugins(ClientTools Tools PipelineLoader TextureLoader FontLoader)
target_link_libraries(ClientTools PUBLIC FilesystemSerialize)


project(ClientNodesTools)
set(sources "")
FolderSources(sources "" nodes_tools)
	FolderSources(sources nodes_tools Madgine clientnodestoolsforward.h clientnodestoolslib.h)
		FolderSources(sources Madgine client)
			FolderSources(sources client nodes noderenderertester.cpp noderenderertester.h)

add_tools(ClientNodes ${sources} PRECOMPILED_HEADER nodes_tools/Madgine/clientnodestoolslib.h SOURCE_ROOT nodes_tools)

target_link_plugins(ClientNodesTools Tools ClientTools)


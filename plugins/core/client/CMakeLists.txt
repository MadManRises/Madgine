

Project(Client)     

set(sources "")
FolderSources(sources "" src)
	FolderSources(sources src Madgine clientforward.h clientlib.h)
		FolderSources(sources Madgine input inputcollector.cpp inputcollector.h inputevents.h inputhandler.cpp inputhandler.h inputlistener.h)
		FolderSources(sources Madgine render camera.cpp camera.h rendercontext.cpp rendercontext.h rendertarget.cpp rendertarget.h rendercontextcollector.cpp rendercontextcollector.h)	
		FolderSources(sources Madgine threading framelistener.h frameloop.cpp frameloop.h)
		FolderSources(sources Madgine window mainwindow.cpp mainwindow.h mainwindowcomponent.cpp mainwindowcomponent.h mainwindowcomponentcollector.cpp mainwindowcomponentcollector.h toolwindow.cpp toolwindow.h)

add_plugin(Client Madgine Core ${sources} PRECOMPILED_HEADER src/Madgine/clientlib.h)

target_link_plugins(Client MeshLoader ImageLoader ProgramLoader TextureLoader FontLoader)


set(sources "")
FolderSources(sources "" tools)
	FolderSources(sources tools Madgine_Tools)
		FolderSources(sources Madgine_Tools im3d im3drenderpass.cpp im3drenderpass.h)
		FolderSources(sources Madgine_Tools imgui clientimroot.cpp clientimroot.h)

add_tools(Client ${sources} PRECOMPILED_HEADER tools/Madgine_Tools/clienttoolslib.h)

target_link_plugins(ClientTools Tools)


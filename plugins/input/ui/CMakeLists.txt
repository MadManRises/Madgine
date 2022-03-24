

Project(UI)     

set(sources "")
FolderSources(sources "" src)
	FolderSources(sources src Madgine uiforward.h uilib.h)
		FolderSources(sources Madgine input gamehandler.cpp gamehandler.h guihandler.cpp guihandler.h handler.cpp handler.h uimanager.cpp uimanager.h)

add_plugin(UI Madgine Input ${sources} PRECOMPILED_HEADER src/Madgine/uilib.h)

target_link_plugins(UI Widgets)



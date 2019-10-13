

Project(Client)     

set(sources "")
FolderSources(sources "" src)
	FolderSources(sources src Madgine clientforward.h clientlib.h)
		FolderSources(sources Madgine gui)
			FolderSources(sources gui widgets bar.cpp bar.h button.cpp button.h checkbox.cpp checkbox.h classid.h combobox.cpp combobox.h image.cpp image.h label.cpp label.h scenewindow.cpp scenewindow.h tabwidget.cpp tabwidget.h textbox.cpp textbox.h toolwindow.cpp toolwindow.h toplevelwindow.cpp toplevelwindow.h widget.cpp widget.h widgetclass.h)
		FolderSources(sources Madgine input inputcollector.cpp inputcollector.h inputevents.h inputhandler.cpp inputhandler.h inputlistener.h)
		FolderSources(sources Madgine render renderwindow.cpp renderwindow.h rendertarget.cpp rendertarget.h renderwindowcollector.cpp renderwindowcollector.h)	
		FolderSources(sources Madgine ui gamehandler.cpp gamehandler.h guihandler.cpp guihandler.h handler.cpp handler.h uimanager.cpp uimanager.h)
		FolderSources(sources Madgine threading framelistener.h frameloop.cpp frameloop.h)

add_plugin(Client Madgine Core ${sources} PRECOMPILED_HEADER src/Madgine/clientlib.h)

target_link_plugins(Client Base ImageLoader)


set(sources "")
FolderSources(sources "" tools)
	FolderSources(sources tools Madgine_Tools)
		FolderSources(sources Madgine_Tools guieditor guieditor.cpp guieditor.h widgetsettings.cpp widgetsettings.h)
		FolderSources(sources Madgine_Tools im3d im3drenderpass.cpp im3drenderpass.h)
		FolderSources(sources Madgine_Tools imgui clientimroot.cpp clientimroot.h immanager.cpp immanager.h)

add_tools(Client ${sources} PRECOMPILED_HEADER tools/Madgine_Tools/clienttoolslib.h)

target_link_plugins(ClientTools Tools ImageLoader)


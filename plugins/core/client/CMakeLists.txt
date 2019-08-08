

Project(Client)     

set(sources "")
FolderSources(sources "" src clientforward.h clientlib.h)
	FolderSources(sources src gui guisystem.cpp guisystem.h)
		FolderSources(sources gui widgets bar.cpp bar.h button.cpp button.h checkbox.cpp checkbox.h classid.h combobox.cpp combobox.h image.cpp image.h label.cpp label.h scenewindow.cpp scenewindow.h tabwidget.cpp tabwidget.h textbox.cpp textbox.h toolwindow.cpp toolwindow.h toplevelwindow.cpp toplevelwindow.h widget.cpp widget.h widgetclass.h)
	FolderSources(sources src input inputcollector.cpp inputcollector.h inputevents.h inputhandler.cpp inputhandler.h inputlistener.h)
	FolderSources(sources src render renderercollector.cpp renderercollector.h rendererbase.cpp rendererbase.h renderwindow.cpp renderwindow.h rendertarget.cpp rendertarget.h)	
	FolderSources(sources src ui gamehandler.cpp gamehandler.h guihandler.cpp guihandler.h handler.cpp handler.h uimanager.cpp uimanager.h)



add_plugin(Client Madgine Core ${sources} PRECOMPILED_HEADER src/clientlib.h)

target_link_libraries(Client PUBLIC Base)


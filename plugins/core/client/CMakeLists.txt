

Project(Client)     

set(sources "")
FolderSources(sources "" "" clientforward.h clientlib.h)
	FolderSources(sources "" gui guisystem.cpp guisystem.h)
		FolderSources(sources gui widgets bar.cpp bar.h button.cpp button.h checkbox.cpp checkbox.h classid.h combobox.cpp combobox.h image.cpp image.h label.cpp label.h scenewindow.cpp scenewindow.h tabwidget.cpp tabwidget.h textbox.cpp textbox.h toplevelwindow.cpp toplevelwindow.h widget.cpp widget.h widgetclass.h)
	FolderSources(sources "" input inputhandler.cpp inputhandler.h)
	FolderSources(sources "" render renderercollector.h rendererbase.cpp rendererbase.h renderwindow.cpp renderwindow.h rendertarget.cpp rendertarget.h)	
	FolderSources(sources "" ui gamehandler.cpp gamehandler.h guihandler.cpp guihandler.h handler.cpp handler.h uimanager.cpp uimanager.h)



add_plugin(Client Madgine Core ${sources} SOURCE_ROOT .. PRECOMPILED_HEADER clientlib.h)

target_link_libraries(Client PUBLIC Base)


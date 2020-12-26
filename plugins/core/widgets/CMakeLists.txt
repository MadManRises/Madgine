

Project(Widgets)     

set(sources "")
FolderSources(sources "" src)
	FolderSources(sources src Madgine widgetsforward.h widgetslib.h)
		FolderSources(sources Madgine widgets bar.cpp bar.h button.cpp button.h checkbox.cpp checkbox.h classid.h combobox.cpp combobox.h image.cpp image.h label.cpp label.h scenewindow.cpp scenewindow.h tabwidget.cpp tabwidget.h textbox.cpp textbox.h widget.cpp widget.h widgetclass.h widgetmanager.cpp widgetmanager.h)

add_plugin(Widgets Madgine Core ${sources} PRECOMPILED_HEADER src/Madgine/widgetslib.h)

target_link_plugins(Widgets Client ImageLoader FontLoader MeshLoader ProgramLoader)


set(sources "")
FolderSources(sources "" tools)
	FolderSources(sources tools Madgine_Tools)
		FolderSources(sources Madgine_Tools guieditor guieditor.cpp guieditor.h widgetsettings.cpp widgetsettings.h)

add_tools(Widgets ${sources} PRECOMPILED_HEADER tools/Madgine_Tools/widgetstoolslib.h)

target_link_plugins(WidgetsTools ClientTools)
target_link_libraries(WidgetsTools PUBLIC Xml)


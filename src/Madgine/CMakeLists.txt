cmake_minimum_required (VERSION 2.6)

cmake_policy(SET CMP0049 OLD)
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0022 NEW)

Project(Base)

set(sources "")
FolderSources(sources "" "" baseforward.h baselib.h)
	FolderSources(sources "" app application.cpp application.h appsettings.h globalapicomponentbase.cpp globalapicomponentbase.h globalapicomponent.h)
	FolderSources(sources "" core framelistener.h frameloop.cpp frameloop.h madgineobject.cpp madgineobject.h objectstate.h root.cpp root.h)
	FolderSources(sources "" resources resource.h resourcebase.cpp resourcebase.h resourceloaderbase.cpp resourceloaderbase.h resourceloadercollector.h resourceloader.h resourcemanager.cpp resourcemanager.h)
		FolderSources(sources resources scripts scriptloader.cpp scriptloader.h)
	FolderSources(sources "" scene contextmasks.h light.cpp light.h lightmanager.cpp lightmanager.h scenemanager.cpp scenemanager.h scenecomponent.h scenecomponentcollector.h scenecomponentbase.cpp scenecomponentbase.h scenecomponentset.h)
		FolderSources(sources scene entity entitycomponentbase.cpp entitycomponentbase.h entitycomponentcollector.cpp entitycomponentcollector.h componentexception.h entity.cpp entity.h entitycomponent.h masks.h)
			FolderSources(sources entity components animation.cpp animation.h mesh.cpp mesh.h transform.cpp transform.h)
	FolderSources(sources "" server serverbase.cpp serverbase.h serverappinstance.cpp serverappinstance.h serverlog.cpp serverlog.h)
	FolderSources(sources "" uniquecomponent indexholder.h uniquecomponent.h uniquecomponentcollector.h uniquecomponentcontainer.h uniquecomponentcollectormanager.cpp uniquecomponentcollectormanager.h uniquecomponentregistry.cpp uniquecomponentregistry.h)


add_workspace_library(Base ${sources} SOURCE_ROOT ..)
add_precompiled_header(Base baselib.h FORCEINCLUDE)


target_link_libraries(Base PUBLIC Interfaces)

target_link_libraries(Base PRIVATE ${CMAKE_THREAD_LIBS_INIT})





Project(Client)     

set(sources "")
FolderSources(sources "" "" clientforward.h clientlib.h)
	FolderSources(sources "" app clientapplication.cpp clientapplication.h clientappsettings.h)
	FolderSources(sources "" gui guisystem.cpp guisystem.h)
		FolderSources(sources gui widgets bar.cpp bar.h button.cpp button.h checkbox.cpp checkbox.h classid.h combobox.cpp combobox.h image.cpp image.h label.cpp label.h scenewindow.cpp scenewindow.h tabwidget.cpp tabwidget.h textbox.cpp textbox.h toplevelwindow.cpp toplevelwindow.h widget.cpp widget.h widgetclass.h)
	FolderSources(sources "" input inputhandler.cpp inputhandler.h)
	FolderSources(sources "" render camera.cpp camera.h renderer.h rendererbase.cpp rendererbase.h renderwindow.cpp renderwindow.h rendertarget.cpp rendertarget.h)	
	FolderSources(sources "" ui gamehandler.cpp gamehandler.h guihandler.cpp guihandler.h handler.cpp handler.h uimanager.cpp uimanager.h)



add_workspace_library(Client ${sources} SOURCE_ROOT ..)

add_precompiled_header(Client clientlib.h FORCEINCLUDE)

target_link_libraries(Client PUBLIC Base)

target_compile_definitions(Client INTERFACE MADGINE_CLIENT_BUILD)




install_to_workspace(Madgine TARGETS Base Client EXPORT_LIB)

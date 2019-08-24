cmake_minimum_required (VERSION 2.6)

Project(Base)

set(sources "")
FolderSources(sources "" src)
	FolderSources(sources src Madgine baseforward.h baselib.h)
		FolderSources(sources Madgine app application.cpp application.h appsettings.h globalapibase.cpp globalapibase.h globalapicollector.cpp globalapicollector.h)
		FolderSources(sources Madgine core madgineobject.cpp madgineobject.h madgineobjectobserver.cpp madgineobjectobserver.h objectstate.h root.cpp root.h)
		FolderSources(sources Madgine resources resource.h resourcebase.cpp resourcebase.h resourceloaderbase.cpp resourceloaderbase.h resourceloadercollector.cpp resourceloadercollector.h resourceloader.h resourcemanager.cpp resourcemanager.h)
			#FolderSources(sources resources scripts scriptloader.cpp scriptloader.h)
		FolderSources(sources Madgine scene camera.cpp camera.h contextmasks.h light.cpp light.h lightmanager.cpp lightmanager.h scenemanager.cpp scenemanager.h scenecomponentcollector.cpp scenecomponentcollector.h scenecomponentbase.cpp scenecomponentbase.h scenecomponentset.h)
			FolderSources(sources scene entity entitycomponentbase.cpp entitycomponentbase.h entitycomponentcollector.cpp entitycomponentcollector.h componentexception.h entity.cpp entity.h entitycomponent.h masks.h)
				FolderSources(sources entity components animation.cpp animation.h mesh.cpp mesh.h transform.cpp transform.h)
		FolderSources(sources Madgine threading framelistener.h frameloop.cpp frameloop.h)

add_plugin(Base Madgine Core ${sources} PRECOMPILED_HEADER src/Madgine/baselib.h)

target_link_libraries(Base PUBLIC Modules)

target_link_libraries(Base PRIVATE ${CMAKE_THREAD_LIBS_INIT})


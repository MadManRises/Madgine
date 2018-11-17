cmake_minimum_required (VERSION 2.6)

cmake_policy(SET CMP0049 OLD)
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0022 NEW)

Project(Base)

set(sources "")
FolderSources(sources "" "../../src")
FolderSources(sources "../../src" Madgine baseforward.h baselib.h)
	FolderSources(sources Madgine app application.cpp application.h appsettings.h globalapicomponentbase.cpp globalapicomponentbase.h globalapicomponent.h)
	FolderSources(sources Madgine core framelistener.h frameloop.cpp frameloop.h madgineobject.cpp madgineobject.h objectstate.h root.cpp root.h)
	FolderSources(sources Madgine resources resource.h resourcebase.cpp resourcebase.h resourceloaderbase.cpp resourceloaderbase.h resourceloadercollector.h resourceloader.h resourcemanager.cpp resourcemanager.h)
		FolderSources(sources resources scripts scriptloader.cpp scriptloader.h)
	FolderSources(sources Madgine scene contextmasks.h light.cpp light.h lightmanager.cpp lightmanager.h scenemanager.cpp scenemanager.h scenecomponent.h scenecomponentcollector.h scenecomponentbase.cpp scenecomponentbase.h scenecomponentset.h)
		FolderSources(sources scene entity entitycomponentbase.cpp entitycomponentbase.h entitycomponentcollector.cpp entitycomponentcollector.h componentexception.h entity.cpp entity.h entitycomponent.h masks.h)
			FolderSources(sources entity components animation.cpp animation.h mesh.cpp mesh.h transform.cpp transform.h)
	FolderSources(sources Madgine server serverbase.cpp serverbase.h serverappinstance.cpp serverappinstance.h serverlog.cpp serverlog.h)
	FolderSources(sources Madgine uniquecomponent uniquecomponent.h uniquecomponentcollector.h uniquecomponentcontainer.h uniquecomponentcollectormanager.cpp uniquecomponentcollectormanager.h uniquecomponentregistry.cpp uniquecomponentregistry.h)


add_workspace_library(Base  ${LIBRARY_TYPE} ${sources})
add_precompiled_header(Base ../../src/Madgine/baselib.h FORCEINCLUDE)

target_link_libraries(Base PUBLIC Interfaces)

target_link_libraries(Base PRIVATE ${CMAKE_THREAD_LIBS_INIT})

if (BUILD_TESTING)
	add_subdirectory(test)
endif()


install_to_workspace(Madgine TARGETS Base EXPORT_LIB)

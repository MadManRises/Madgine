cmake_minimum_required (VERSION 2.6)

Project(Base)

set(sources "")
FolderSources(sources "" "" baseforward.h baselib.h)
	FolderSources(sources "" app application.cpp application.h appsettings.h globalapibase.cpp globalapibase.h globalapicollector.cpp globalapicollector.h)
	FolderSources(sources "" core madgineobject.cpp madgineobject.h madgineobjectobserver.cpp madgineobjectobserver.h objectstate.h root.cpp root.h)
	FolderSources(sources "" resources resource.h resourcebase.cpp resourcebase.h resourceloaderbase.cpp resourceloaderbase.h resourceloadercollector.cpp resourceloadercollector.h resourceloader.h resourcemanager.cpp resourcemanager.h)
		FolderSources(sources resources scripts scriptloader.cpp scriptloader.h)
	FolderSources(sources "" scene camera.cpp camera.h contextmasks.h light.cpp light.h lightmanager.cpp lightmanager.h scenemanager.cpp scenemanager.h scenecomponentcollector.cpp scenecomponentcollector.h scenecomponentbase.cpp scenecomponentbase.h scenecomponentset.h)
		FolderSources(sources scene entity entitycomponentbase.cpp entitycomponentbase.h entitycomponentcollector.cpp entitycomponentcollector.h componentexception.h entity.cpp entity.h entitycomponent.h masks.h)
			FolderSources(sources entity components animation.cpp animation.h mesh.cpp mesh.h transform.cpp transform.h)
	FolderSources(sources "" server serverbase.cpp serverbase.h serverappinstance.cpp serverappinstance.h serverlog.cpp serverlog.h)


add_workspace_library(Base ${sources} SOURCE_ROOT .. PRECOMPILED_HEADER baselib.h)

target_link_libraries(Base PUBLIC Modules)

target_compile_definitions(Base INTERFACE BUILD_Base)

target_link_libraries(Base PRIVATE ${CMAKE_THREAD_LIBS_INIT})

generate_binary_info(Base)


install_to_workspace(Madgine TARGETS Base EXPORT_LIB)

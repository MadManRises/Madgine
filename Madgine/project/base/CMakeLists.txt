cmake_minimum_required (VERSION 2.6)

cmake_policy(SET CMP0049 OLD)
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0022 NEW)

Project(Base)

set(sources "")
FolderSources(sources "" "../../src")
FolderSources(sources "../../src" Madgine baseforward.h baselib.h madgineobject.cpp madgineobject.h objectstate.h uniquecomponent.h uniquecomponentcollector.h)
	FolderSources(sources Madgine app application.cpp application.h appsettings.h framelistener.h root.cpp root.h)
	FolderSources(sources Madgine resources resource.h resourcebase.cpp resourcebase.h resourceloaderbase.cpp resourceloaderbase.h resourceloadercollector.h resourceloader.h resourcemanager.cpp resourcemanager.h)
	FolderSources(sources Madgine scene contextmasks.h light.cpp light.h scenemanager.h scenemanagerbase.cpp scenemanagerbase.h scenecomponent.h scenecomponentcollector.h scenecomponentbase.cpp scenecomponentbase.h scenecomponentset.h)
		FolderSources(sources scene entity entitycomponentbase.cpp entitycomponentbase.h componentexception.h entity.cpp entity.h entitycomponent.h masks.h)
			FolderSources(sources entity components animation.cpp animation.h mesh.cpp mesh.h transform.cpp transform.h)
	FolderSources(sources Madgine scripting)
		FolderSources(sources scripting types globalapicomponentbase.cpp globalapicomponentbase.h globalapicomponent.h globalscopebase.cpp globalscopebase.h)
		FolderSources(sources scripting parsing scriptloader.cpp scriptloader.h)
	FolderSources(sources Madgine serialize toplevelids.h)
	FolderSources(sources Madgine util processstats.cpp processstats.h profile.cpp profile.h profiler.cpp profiler.h)


add_library(Base SHARED ${sources} baselib.cpp)
add_precompiled_header(Base ../../src/Madgine/baselib.h FORCEINCLUDE SOURCE_CXX baselib.cpp)

target_link_libraries(Base PUBLIC Interfaces)

if (BUILD_TESTING)
	add_subdirectory(test)
endif()


install_to_workspace(Madgine TARGETS Base EXPORT_LIB)

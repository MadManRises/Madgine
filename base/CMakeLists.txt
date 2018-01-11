cmake_minimum_required (VERSION 2.6)

cmake_policy(SET CMP0049 OLD)
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0022 NEW)

Project(BASE)

set(sources "")
FolderSources(sources "" "")
FolderSources(sources "" src baseforward.h baselib.h madgineobject.cpp madgineobject.h madgineobjectcollector.cpp madgineobjectcollector.h objectstate.h uniquecomponent.h uniquecomponentcollector.h)
	FolderSources(sources src app application.cpp application.h appsettings.h framelistener.h)
	FolderSources(sources src scene contextmasks.h light.cpp light.h scenemanager.h scenemanagerbase.cpp scenemanagerbase.h scenecomponent.h scenecomponentcollector.h scenecomponentbase.cpp scenecomponentbase.h scenecomponentset.h)
		FolderSources(sources scene entity entitycomponentbase.cpp entitycomponentbase.h componentexception.h entity.cpp entity.h entitycomponent.h masks.h)
			FolderSources(sources entity components animation.h mesh.h transform.h)
	FolderSources(sources src scripting)
		FolderSources(sources scripting types globalapicomponentbase.cpp globalapicomponentbase.h globalapicomponent.cpp globalapicomponent.h )
	FolderSources(sources src serialize toplevelids.h)
	FolderSources(sources src util processstats.cpp processstats.h profile.cpp profile.h profiler.cpp profiler.h)

include_directories(${BASE_SOURCE_DIR}/src)
add_library(Base SHARED ${sources} baselib.cpp)
add_precompiled_header(Base src/baselib.h FORCEINCLUDE SOURCE_CXX baselib.cpp)

include_targets(Base Interfaces)

 
if (BUILD_TESTS)
	add_subdirectory(test)
endif(BUILD_TESTS)

installToWorkspace(Base Madgine)

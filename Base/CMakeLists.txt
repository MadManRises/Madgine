cmake_minimum_required (VERSION 2.6)

cmake_policy(SET CMP0011 OLD)
cmake_policy(SET CMP0049 OLD)
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0022 NEW)

Project(BASE)

set(sources "")
FolderSources(sources "" "")
FolderSources(sources "" src baseforward.h baselib.h MadgineObject.cpp MadgineObject.h MadgineObjectCollector.cpp MadgineObjectCollector.h ObjectState.h uniquecomponent.h uniquecomponentcollector.h)
	FolderSources(sources src App application.cpp application.h appsettings.h framelistener.h)
	FolderSources(sources src Os os.h windows.cpp)
	FolderSources(sources src Scene contextmasks.h light.cpp light.h scenemanager.cpp scenemanager.h scenecomponent.cpp scenecomponent.h)
		FolderSources(sources Scene Entity EntityComponentBase.cpp EntityComponentBase.h componentexception.h entity.cpp entity.h entitycomponent.h masks.h)
	FolderSources(sources src Scripting)
		FolderSources(sources Scripting Types globalapicomponentbase.cpp globalapicomponentbase.h globalapi.h globalapicomponent.h GlobalScope.cpp GlobalScope.h scene.cpp scene.h scriptingmanager.cpp scriptingmanager.h)
	FolderSources(sources src Serialize TopLevelIds.h)

include_directories(${BASE_SOURCE_DIR}/src)
add_library(Base SHARED ${sources} baselib.cpp)
add_precompiled_header(Base src/baselib.h FORCEINCLUDE SOURCE_CXX baselib.cpp)

include_library(Base Interfaces Interfaces)

 
if (BUILD_TESTS)
	add_subdirectory(test)
endif(BUILD_TESTS)

installToWorkspace(Base Madgine)

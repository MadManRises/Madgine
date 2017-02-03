cmake_minimum_required (VERSION 2.6)
cmake_policy(SET CMP0011 OLD)
cmake_policy(SET CMP0049 OLD)


Project(BASE)

set(CMAKE_MODULE_PATH
  "${CMAKE_MODULE_PATH}"
)


set(sources "")
FolderSources(sources "" "")
FolderSources(sources "" src baseforward.h baselib.h uniquecomponent.h uniquecomponentcollector.h)
	FolderSources(sources src App contextmasks.h)
	FolderSources(sources src Scene scenemanager.cpp scenemanager.h scenecomponent.cpp scenecomponent.h)
	FolderSources(sources src Scripting)
		FolderSources(sources Scripting Types baseglobalapicomponent.cpp baseglobalapicomponent.h globalapi.h globalapicomponent.h globalscopeimpl.cpp globalscopeimpl.h scene.cpp scene.h scriptingmanager.cpp scriptingmanager.h)
	FolderSources(sources src Util MadgineObject.cpp MadgineObject.h MadgineObjectCollector.cpp MadgineObjectCollector.h ObjectState.h)

include_directories(${BASE_SOURCE_DIR}/src ${INTERFACES_SOURCE_DIR}/src)

add_library(Base SHARED ${sources} baselib.cpp)

target_link_libraries(Base Interfaces)

add_precompiled_header(Base src/baselib.h FORCEINCLUDE SOURCE_CXX baselib.cpp)

 
if (BUILD_TESTS)
	add_subdirectory(test)
endif(BUILD_TESTS)
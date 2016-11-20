cmake_minimum_required (VERSION 2.6)

cmake_policy(SET CMP0011 OLD)
cmake_policy(SET CMP0049 OLD)


PROJECT(Maditor_Madgine_Launcher)


set (sources "")
FolderSources(sources "" "" madgineinclude.cpp madgineinclude.h main.cpp)
	FolderSources(sources "" Base ApplicationLauncher.cpp ApplicationLauncher.h ApplicationWatcher.cpp ApplicationWatcher.h InputWrapper.cpp InputWrapper.h ModuleLoader.cpp ModuleLoader.h)
	FolderSources(sources "" Watcher LogWatcher.cpp LogWatcher.h ObjectsWatcher.cpp ObjectsWatcher.h PerformanceWatcher.cpp PerformanceWatcher.h)

include_directories(${MADGINE_SOURCE_DIR}/src)
include_directories(${Maditor_SOURCE_DIR}/Common)

if (OGRE_FOUND)
  include_directories(${OGRE_INCLUDE_DIRS} ${OGRE_Terrain_INCLUDE_DIRS} ${OGRE_RTShaderSystem_INCLUDE_DIRS})
endif (OGRE_FOUND)

add_executable(Maditor_Madgine_Launcher ${sources})

add_precompiled_header(Maditor_Madgine_Launcher madgineinclude.h FORCEINCLUDE SOURCE_CXX madgineinclude.cpp)

target_link_libraries(Maditor_Madgine_Launcher Madgine)

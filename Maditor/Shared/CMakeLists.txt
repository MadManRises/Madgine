cmake_minimum_required (VERSION 2.6)

cmake_policy(SET CMP0011 OLD)
cmake_policy(SET CMP0049 OLD)


PROJECT(Maditor_Shared)


set (sources "")
FolderSources(sources "" "" AppControl.cpp)
	

include_directories(${MADGINE_SOURCE_DIR}/src)

if (OGRE_FOUND)
  include_directories(${OGRE_INCLUDE_DIRS} ${OGRE_Terrain_INCLUDE_DIRS} ${OGRE_RTShaderSystem_INCLUDE_DIRS})
endif (OGRE_FOUND)

add_library(Maditor_Shared SHARED ${sources})

#add_precompiled_header(Maditor_Shared madgineinclude.h FORCEINCLUDE SOURCE_CXX madgineinclude.cpp)

target_link_libraries(Maditor_Shared Madgine)

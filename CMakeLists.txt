cmake_minimum_required (VERSION 3.3)

cmake_policy(SET CMP0048 NEW)

set (CMAKE_CXX_STANDARD 17)

add_definitions(-D_HAS_TR1_NAMESPACE)

set(MADGINE_MAJOR_VERSION 0)
set(MADGINE_MINOR_VERSION 1)
set(MADGINE_PATCH_VERSION 0)

project(Madgine VERSION ${MADGINE_MAJOR_VERSION}.${MADGINE_MINOR_VERSION}.${MADGINE_PATCH_VERSION})

if (UNIX)
	add_definitions(-D__STDC_VERSION__=201112L) #fix for luaconf.h
endif()

set(BUILD_OGRE TRUE CACHE BOOL "Build OgreApplication")

set(CMAKE_MODULE_PATH
	"${CMAKE_MODULE_PATH}"
	"${CMAKE_SOURCE_DIR}/cmake"
)

include(Workspace)
include(PrecompiledHeader)
include(Project)
include(multiConfig)
  
multiConfig(Madgine)  

include(Testing)

add_subdirectory(interfaces)
add_subdirectory(base)
if (BUILD_OGRE AND NOT CONFIG_BUILD)
	add_subdirectory(ogre)
endif ()
add_subdirectory(server)


export_to_workspace(Madgine)

configure_file(doc/Doxyfile.in Doxyfile)

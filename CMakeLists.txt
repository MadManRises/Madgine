
cmake_minimum_required (VERSION 3.8)

cmake_policy(SET CMP0048 NEW)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


set(MADGINE_MAJOR_VERSION 0)
set(MADGINE_MINOR_VERSION 1)
set(MADGINE_PATCH_VERSION 0)

project(Madgine VERSION ${MADGINE_MAJOR_VERSION}.${MADGINE_MINOR_VERSION}.${MADGINE_PATCH_VERSION})

set(CMAKE_MODULE_PATH
	"${CMAKE_MODULE_PATH}"
	"${CMAKE_SOURCE_DIR}/cmake"
)

if (UNIX)
	add_definitions(-D__STDC_VERSION__=201112L) #fix for luaconf.h
endif()

set(BUILD_CLIENT TRUE CACHE BOOL "Build Client Code")


include(Workspace)
include(Packaging)
include(PrecompiledHeader)
include(Project)
include(multiConfig)
include(Plugins)
include(Testing)

set (CPACK_NSIS_MODIFY_PATH ON)

enable_packaging()

multiConfig(Madgine) 

add_subdirectory(Madgine)
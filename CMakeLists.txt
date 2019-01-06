
cmake_minimum_required (VERSION 3.8)

MESSAGE(STATUS ${CMAKE_C_COMPILER})

cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0011 OLD)
cmake_policy(SET CMP0049 OLD)

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
	"${CMAKE_SOURCE_DIR}/cmake/modules"
)


if (UNIX)
	add_definitions(-D__STDC_VERSION__=201112L) #fix for luaconf.h
endif()


option(ENABLE_MEMTRACKING "Enables Mempry Tracking for Heap-Allocations")

if (ENABLE_MEMTRACKING)
	add_definitions(-DENABLE_MEMTRACKING)
endif()


include(Workspace)
include(Packaging)
include(PrecompiledHeader)
include(Project)
include(multiConfig)
include(Plugins)
include(Testing)

set (CPACK_NSIS_MODIFY_PATH ON)
set (CPACK_PACKAGE_VENDOR "MadMan Studios")

#set (CPACK_PACKAGE_EXECUTABLES "Maditor;Maditor")

enable_packaging()

multiConfig(Madgine) 



add_subdirectory(src)

add_subdirectory(plugins)

add_subdirectory(launcher)


if (BUILD_TESTING)
	add_subdirectory(test)
endif()


export_to_workspace(Madgine)

cpack_add_component(Madgine REQUIRED)

cpack_add_component_group(Renderer EXPANDED)

configure_file(doc/Doxyfile.in Doxyfile)


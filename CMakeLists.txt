
cmake_minimum_required (VERSION 3.8)

cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0011 OLD)
cmake_policy(SET CMP0049 OLD)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_definitions(-D_HAS_TR1_NAMESPACE)


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

load_workspace_package(lua https://github.com/MadManRises/lua.git)



add_subdirectory(Madgine)
add_subdirectory(Maditor)
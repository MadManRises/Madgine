
cmake_minimum_required (VERSION 3.8)

cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0011 NEW)
cmake_policy(SET CMP0049 NEW)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


set(MADGINE_MAJOR_VERSION 0)
set(MADGINE_MINOR_VERSION 1)
set(MADGINE_PATCH_VERSION 0)

project(Madgine VERSION ${MADGINE_MAJOR_VERSION}.${MADGINE_MINOR_VERSION}.${MADGINE_PATCH_VERSION})

list(APPEND CMAKE_MODULE_PATH
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake"
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules"
)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} PARENT_SCOPE)



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

add_subdirectory(shared)

add_subdirectory(plugins)

add_subdirectory(launcher)


if (BUILD_TESTING)
	add_subdirectory(test)
endif()


export_to_workspace(Madgine)

cpack_add_component(Madgine REQUIRED)

cpack_add_component_group(Renderer EXPANDED)

configure_file(docs/Doxyfile.in Doxyfile)


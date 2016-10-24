cmake_minimum_required (VERSION 3.3)

set(OGRE_SDK "" CACHE PATH "Path to Ogre install directory")
set(BUILD_MADITOR TRUE CACHE BOOL "Include the Maditor-build")


Project(Madgine)

if (MINGW)
	add_compile_options(-Wall -std=c++14 -fpermissive)
endif (MINGW)

set(CMAKE_MODULE_PATH
	"${CMAKE_SOURCE_DIR}/CMake"
	"${OGRE_SDK}/CMake"
)

include(PrecompiledHeader)
INCLUDE(Project)

find_package(OGRE REQUIRED)

add_subdirectory(Madgine)
if (BUILD_MADITOR)
	add_subdirectory(Maditor)
endif (BUILD_MADITOR)
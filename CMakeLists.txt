set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)

cmake_minimum_required (VERSION 3.3)

set(OGRE_SDK "" CACHE PATH "Path to Ogre install directory")
set(BUILD_MADITOR TRUE CACHE BOOL "Include the Maditor-build")

set(BUILD_TESTS FALSE CACHE BOOL "Builds the tests")

if (MINGW)
	add_compile_options(-Wall -std=c++14 -fpermissive)
endif (MINGW)

Project(Madgine)



set(CMAKE_MODULE_PATH
	"${CMAKE_SOURCE_DIR}/CMake"
	"${OGRE_SDK}/CMake"
)

include(PrecompiledHeader)
include(Project)


find_package(OGRE REQUIRED)


if (BUILD_TESTS)
	add_subdirectory(gtest1.8.0)
endif (BUILD_TESTS)

add_subdirectory(Madgine)
if (BUILD_MADITOR)
	add_subdirectory(Maditor)
endif (BUILD_MADITOR)
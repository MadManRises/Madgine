set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/../bin)

cmake_minimum_required (VERSION 3.3)

cmake_policy(SET CMP0048 NEW)

set (CMAKE_CXX_STANDARD 14)

set(MADGINE_MAJOR_VERSION 0)
set(MADGINE_MINOR_VERSION 1)
set(MADGINE_PATCH_VERSION 0)

project(Madgine VERSION ${MADGINE_MAJOR_VERSION}.${MADGINE_MINOR_VERSION}.${MADGINE_PATCH_VERSION})
                            


set(BUILD_TESTS FALSE CACHE BOOL "Builds the tests")


set(CMAKE_MODULE_PATH
	"${CMAKE_MODULE_PATH}"
	"${CMAKE_SOURCE_DIR}/cmake"
)

include(Workspace)
include(PrecompiledHeader)
include(Project)
include(multiConfig)
  
multiConfig(Madgine)  


if (BUILD_TESTS)
	add_subdirectory(gtest1.8.0)
endif (BUILD_TESTS)


add_subdirectory(Base)
add_subdirectory(Ogre)
#add_subdirectory(Unity)
add_subdirectory(Server)


exportToWorkspace(Madgine)
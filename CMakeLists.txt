cmake_minimum_required (VERSION 2.6)

set(OGRE_PATH "" CACHE PATH "Path to FindOgre.cmake ans FindOIS.cmake")


set(CMAKE_MODULE_PATH
	"${CMAKE_SOURCE_DIR}/CMake"
	"${OGRE_PATH}"
)

include(PrecompiledHeader)
INCLUDE(Project)

find_package(OGRE REQUIRED)

add_subdirectory(Madgine)
add_subdirectory(Maditor)
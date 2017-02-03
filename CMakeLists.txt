set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)

cmake_minimum_required (VERSION 3.3)

project(Madgine)

set(MADGINE_MAJOR_VERSION 0)
set(MADGINE_MINOR_VERSION 1)
set(MADGINE_PATCH_VERSION 0)
set(MADGINE_VERSION
  ${MADGINE_MAJOR_VERSION}.${MADGINE_MINOR_VERSION}.${MADGINE_PATCH_VERSION})



set(BUILD_TESTS FALSE CACHE BOOL "Builds the tests")

if (MINGW)
	add_compile_options(-Wall -std=c++14 -fpermissive)
endif (MINGW)

if (NOT APPLE)
  # Create debug libraries with _d postfix
  set(CMAKE_DEBUG_POSTFIX "_d")
endif ()



set(CMAKE_MODULE_PATH
	"${CMAKE_SOURCE_DIR}/CMake"
)

include(PrecompiledHeader)
include(Project)


include_directories(Common)

if (BUILD_TESTS)
	add_subdirectory(gtest1.8.0)
endif (BUILD_TESTS)

add_subdirectory(Interfaces)
add_subdirectory(Base)
add_subdirectory(Madgine)


# Export Configuration
# ===============================
 
# Add all targets to the build-tree export set
export(TARGETS Madgine Interfaces Base
  FILE "${PROJECT_BINARY_DIR}/MadgineTargets.cmake")
 
# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE Madgine)
 
# Create the MadgineConfig.cmake and MadgineConfigVersion files
#file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}"
#   "${INSTALL_INCLUDE_DIR}")
# ... for the build tree
get_property(dirs DIRECTORY ${MADGINE_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
set(CONF_INCLUDE_DIRS "${dirs}")
configure_file(CMake/MadgineConfig.cmake.in
  "${PROJECT_BINARY_DIR}/MadgineConfig.cmake" @ONLY)
# ... for the install tree
set(CONF_INCLUDE_DIRS "\${MADGINE_CMAKE_DIR}/${REL_INCLUDE_DIR}")
configure_file(CMake/MadgineConfig.cmake.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/MadgineConfig.cmake" @ONLY)
# ... for both
configure_file(CMake/MadgineConfigVersion.cmake.in
  "${PROJECT_BINARY_DIR}/MadgineConfigVersion.cmake" @ONLY)
 
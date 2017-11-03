cmake_minimum_required (VERSION 3.3)

cmake_policy(SET CMP0048 NEW)

set (CMAKE_CXX_STANDARD 17)

add_definitions(-D_HAS_TR1_NAMESPACE)

set(MADGINE_MAJOR_VERSION 0)
set(MADGINE_MINOR_VERSION 1)
set(MADGINE_PATCH_VERSION 0)

project(Madgine VERSION ${MADGINE_MAJOR_VERSION}.${MADGINE_MINOR_VERSION}.${MADGINE_PATCH_VERSION})

if (MSVC)
	add_compile_options(/std:c++latest)
endif(MSVC)


set(BUILD_TESTS FALSE CACHE BOOL "Builds the tests")
set(BUILD_OGRE TRUE CACHE BOOL "Build OgreApplication")


FIND_PACKAGE ( Threads REQUIRED )

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
	include (CTest)
	
	set (GTEST_DIR ${CMAKE_CURRENT_BINARY_DIR}/googletest)
	set (GTEST_WORK_DIR ${CMAKE_CURRENT_BINARY_DIR})

	include(ExternalProject)
	ExternalProject_Add(googletest
		GIT_REPOSITORY    https://github.com/google/googletest.git
		GIT_TAG           master
		INSTALL_DIR       ${GTEST_DIR}
		CMAKE_ARGS
			-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
	)

	include_directories(${GTEST_DIR}/include)

	if (WIN32)
		set (debug_suffix "d")
	else()
		set (debug_suffix "")
	endif ()

	set (GTEST_LIBS 
		debug ${GTEST_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}gtest${debug_suffix}${CMAKE_STATIC_LIBRARY_SUFFIX} 
		debug ${GTEST_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}gtest_main${debug_suffix}${CMAKE_STATIC_LIBRARY_SUFFIX} 
		optimized ${GTEST_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}gtest${CMAKE_STATIC_LIBRARY_SUFFIX} 
		optimized ${GTEST_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}gtest_main${CMAKE_STATIC_LIBRARY_SUFFIX} 
		${CMAKE_THREAD_LIBS_INIT}
	)

endif (BUILD_TESTS)

add_subdirectory(interfaces)
add_subdirectory(base)
if (BUILD_OGRE)
	add_subdirectory(ogre)
endif (BUILD_OGRE)
add_subdirectory(server)


exportToWorkspace(Madgine)

configure_file(doc/Doxyfile.in Doxyfile)

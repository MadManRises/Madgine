# - Find MyGUI includes and library
#
# This module defines
# MYGUI_INCLUDE_DIRS
# MYGUI_LIBRARIES, the libraries to link against to use MYGUI.
# MYGUI_LIB_DIR, the location of the libraries
# MYGUI_FOUND, If false, do not try to use MYGUI
#
# Copyright © 2007, Matt Williams
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
CMAKE_POLICY(PUSH)
include(FindPkgMacros)
#include(PreprocessorUtils)

set (MYGUISDK "${Workspace}" CACHE PATH "Path to MyGUI-SDK")

# IF (MYGUI_LIBRARIES AND MYGUI_INCLUDE_DIRS)
    # SET(MYGUI_FIND_QUIETLY TRUE)
# ENDIF (MYGUI_LIBRARIES AND MYGUI_INCLUDE_DIRS)

IF (WIN32) #Windows

    MESSAGE(STATUS "Looking for MyGUI")

    #IF(MINGW)
    
    #    FIND_PATH ( MYGUI_INCLUDE_DIRS MyGUI.h PATH_SUFFIXES MYGUI)
    #    FIND_LIBRARY ( MYGUI_LIBRARIES_REL NAMES
    #        libMyGUIEngine${CMAKE_SHARED_LIBRARY_SUFFIX}
    #        HINTS
    #        ${MYGUISDK}
    #        PATH_SUFFIXES "" release relwithdebinfo minsizerel )
    
    #    FIND_LIBRARY ( MYGUI_LIBRARIES_DBG NAMES
    #        libMyGUIEngine_d${CMAKE_SHARED_LIBRARY_SUFFIX}
    #        HINTS
    #        ${MYGUISDK}
    #        PATH_SUFFIXES "" debug )
    
    #    make_library_set ( MYGUI_LIBRARIES )
    
    #    MESSAGE ("${MYGUI_LIBRARIES}")
    #ENDIF(MINGW)

    #SET(MYGUISDK $ENV{MYGUI_HOME})
    IF (MYGUISDK)
        #findpkg_begin ( "MYGUI" )
        MESSAGE(STATUS "Using MyGUI in MyGUI SDK")
        STRING(REGEX REPLACE "[\\]" "/" _MYGUISDK "${MYGUISDK}" )

        SET ( MYGUI_INCLUDE_DIRS "${_MYGUISDK}/include" )
      

        SET ( MYGUI_LIB_DIR ${_MYGUISDK}/lib)

        if ( MYGUI_STATIC )
           set(LIB_SUFFIX "Static")
           find_package(freetype)
        endif ( MYGUI_STATIC )
        
        if ( MINGW )
            set(LIB_PREFIX "lib")
            set(LIB_EXT ".a")
            set(LIB_EXT2 ".dll")
        else ( MINGW )
            set(LIB_PREFIX "")
            set(LIB_EXT ".lib")
            set(LIB_EXT2 "")
        endif ( MINGW )

		if (CMAKE_BUILD_TYPE STREQUAL "Debug")
			set(LIB_EXT "_d${LIB_EXT}")
		endif()

		MESSAGE(STATUS ${MYGUI_LIB_DIR}/${CMAKE_BUILD_TYPE})
		
        find_library ( MYGUI_LIBRARY NAMES ${LIB_PREFIX}MyGUIEngine${LIB_SUFFIX}${LIB_EXT2}${LIB_EXT} HINTS ${MYGUI_LIB_DIR}/${CMAKE_BUILD_TYPE})                   
		find_library ( MYGUI_LIBRARY_OGRE NAMES ${LIB_PREFIX}MyGUI.OgrePlatform${LIB_SUFFIX}${LIB_EXT} HINTS ${MYGUI_LIB_DIR}/${CMAKE_BUILD_TYPE})

		set(MYGUI_LIBRARIES ${MYGUI_LIBRARY} ${MYGUI_LIBRARY_OGRE} CACHE STRING "" FORCE)

        #findpkg_finish ( "MYGUI" )
    ENDIF (MYGUISDK)
ELSE (WIN32) #Unix
    CMAKE_MINIMUM_REQUIRED(VERSION 2.4.7 FATAL_ERROR)
    FIND_PACKAGE(PkgConfig)
    IF(MYGUI_STATIC)
        # don't use pkgconfig on OS X, find freetype & append it's libs to resulting MYGUI_LIBRARIES
        IF (NOT APPLE AND NOT ANDROID)
            PKG_SEARCH_MODULE(MYGUI MYGUIStatic MyGUIStatic)
            IF (MYGUI_INCLUDE_DIRS)
                SET(MYGUI_INCLUDE_DIRS ${MYGUI_INCLUDE_DIRS})
                SET(MYGUI_LIB_DIR ${MYGUI_LIBDIR})
                SET(MYGUI_LIBRARIES ${MYGUI_LIBRARIES} CACHE STRING "")
            ELSE (MYGUI_INCLUDE_DIRS)
                FIND_PATH(MYGUI_INCLUDE_DIRS MyGUI.h PATHS /usr/local/include /usr/include PATH_SUFFIXES MyGUI MYGUI)
                FIND_LIBRARY(MYGUI_LIBRARIES myguistatic PATHS /usr/lib /usr/local/lib)
                SET(MYGUI_LIB_DIR ${MYGUI_LIBRARIES})
                STRING(REGEX REPLACE "(.*)/.*" "\\1" MYGUI_LIB_DIR "${MYGUI_LIB_DIR}")
                STRING(REGEX REPLACE ".*/" "" MYGUI_LIBRARIES "${MYGUI_LIBRARIES}")
            ENDIF (MYGUI_INCLUDE_DIRS)
        ELSE (NOT APPLE AND NOT ANDROID)
            SET(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${MYGUI_DEPENDENCIES_DIR})
            FIND_PACKAGE(Freetype REQUIRED)
            FIND_PATH(MYGUI_INCLUDE_DIRS MyGUI.h PATHS /usr/local/include /usr/include PATH_SUFFIXES MyGUI MYGUI)
            FIND_LIBRARY(MYGUI_LIBRARIES MyGUIEngineStatic PATHS /usr/lib /usr/local/lib ${OPENMW_DEPENDENCIES_DIR})
            SET(MYGUI_LIB_DIR ${MYGUI_LIBRARIES})
            STRING(REGEX REPLACE "(.*)/.*" "\\1" MYGUI_LIB_DIR "${MYGUI_LIB_DIR}")
            STRING(REGEX REPLACE ".*/" "" MYGUI_LIBRARIES "${MYGUI_LIBRARIES}")
        ENDIF (NOT APPLE AND NOT ANDROID)
    ELSE(MYGUI_STATIC)
        PKG_SEARCH_MODULE(MYGUI MYGUI MyGUI)
        IF (MYGUI_INCLUDE_DIRS)
            SET(MYGUI_INCLUDE_DIRS ${MYGUI_INCLUDE_DIRS})
            SET(MYGUI_LIB_DIR ${MYGUI_LIBDIR})
            SET(MYGUI_LIBRARIES ${MYGUI_LIBRARIES} CACHE STRING "")
        ELSE (MYGUI_INCLUDE_DIRS)
            FIND_PATH(MYGUI_INCLUDE_DIRS MyGUI.h PATHS /usr/local/include /usr/include PATH_SUFFIXES MyGUI MYGUI)
            FIND_LIBRARY(MYGUI_LIBRARIES MyGUIEngine PATHS /usr/local/lib /usr/lib)
            SET(MYGUI_LIB_DIR ${MYGUI_LIBRARIES})
            STRING(REGEX REPLACE "(.*)/.*" "\\1" MYGUI_LIB_DIR "${MYGUI_LIB_DIR}")
            STRING(REGEX REPLACE ".*/" "" MYGUI_LIBRARIES "${MYGUI_LIBRARIES}")
        ENDIF (MYGUI_INCLUDE_DIRS)
    ENDIF(MYGUI_STATIC)
ENDIF (WIN32)


#Do some preparation
IF (NOT WIN32) # This does not work on Windows for paths with spaces in them
	SEPARATE_ARGUMENTS(MYGUI_INCLUDE_DIRS)
	SEPARATE_ARGUMENTS(MYGUI_LIBRARIES)
ENDIF (NOT WIN32)


SET(MYGUI_LIBRARIES ${MYGUI_LIBRARIES} ${FREETYPE_LIBRARIES})

SET(MYGUI_INCLUDE_DIRS ${MYGUI_INCLUDE_DIRS} CACHE PATH "")
SET(MYGUI_LIBRARIES ${MYGUI_LIBRARIES} CACHE STRING "")
SET(MYGUI_LIB_DIR ${MYGUI_LIB_DIR} CACHE PATH "")


IF (NOT APPLE OR NOT MYGUI_STATIC) # we need explicit freetype libs only on OS X for static build, for other cases just make it TRUE
    SET(FREETYPE_LIBRARIES TRUE)
ENDIF (NOT APPLE OR NOT MYGUI_STATIC)

IF (MYGUI_INCLUDE_DIRS AND MYGUI_LIBRARIES AND FREETYPE_LIBRARIES)
    SET(MYGUI_FOUND TRUE)
ENDIF (MYGUI_INCLUDE_DIRS AND MYGUI_LIBRARIES AND FREETYPE_LIBRARIES)

IF (MYGUI_FOUND)
    MARK_AS_ADVANCED(MYGUI_LIB_DIR)
    IF (NOT MYGUI_FIND_QUIETLY)
        MESSAGE(STATUS " libraries : ${MYGUI_LIBRARIES} from ${MYGUI_LIB_DIR}")
        MESSAGE(STATUS " includes : ${MYGUI_INCLUDE_DIRS}")
    ENDIF (NOT MYGUI_FIND_QUIETLY)

    find_file(MYGUI_PREQUEST_FILE NAMES MyGUI_Prerequest.h PATHS ${MYGUI_INCLUDE_DIRS}/MYGUI)
    file(READ ${MYGUI_PREQUEST_FILE} MYGUI_TEMP_VERSION_CONTENT)
    #get_preprocessor_entry(MYGUI_TEMP_VERSION_CONTENT MYGUI_VERSION_MAJOR MYGUI_VERSION_MAJOR)
    #get_preprocessor_entry(MYGUI_TEMP_VERSION_CONTENT MYGUI_VERSION_MINOR MYGUI_VERSION_MINOR)
    #get_preprocessor_entry(MYGUI_TEMP_VERSION_CONTENT MYGUI_VERSION_PATCH MYGUI_VERSION_PATCH)
    #set(MYGUI_VERSION "${MYGUI_VERSION_MAJOR}.${MYGUI_VERSION_MINOR}.${MYGUI_VERSION_PATCH}")

    IF (NOT MYGUI_FIND_QUIETLY)
        MESSAGE(STATUS "MyGUI version: ${MYGUI_VERSION}")
    ENDIF (NOT MYGUI_FIND_QUIETLY)
ENDIF (MYGUI_FOUND)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MyGUI DEFAULT_MSG
    MYGUI_INCLUDE_DIRS
    FREETYPE_LIBRARIES
    MYGUI_LIBRARIES)

CMAKE_POLICY(POP)
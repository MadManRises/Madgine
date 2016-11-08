cmake_minimum_required (VERSION 2.6)

set(BLENDER_TOOL FALSE CACHE BOOL "Enables and installs blender tools")
set(VS_TOOL FALSE CACHE BOOL "Enables Visual Studio tools")
set(BOOST_PATH "" CACHE PATH "Path to Boost")

Project(ProcessTalker)

include_directories(${BOOST_PATH})



if (BLENDER_TOOL)
endif (BLENDER_TOOL)

if (VS_TOOL)
  add_subdirectory(VSLink)
endif (VS_TOOL)

set(TOOLS_LIBS ${TOOLS_LIBS} PARENT_SCOPE)

if (NOT TOOLS_LIBS STREQUAL "")
	add_library(ProcessTalker STATIC ProcessTalker.cpp ProcessTalker.h)
endif()

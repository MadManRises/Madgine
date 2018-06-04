cmake_minimum_required (VERSION 2.6)

option(BLENDER_TOOL "Enables and installs blender tools")
option(VS_TOOL "Enables Visual Studio tools")


Project(Tools)


set (TOOLS_LIBS "")

set (TOOLS_BINS "")

if (BLENDER_TOOL)
endif (BLENDER_TOOL)

if (VS_TOOL)
  add_subdirectory(VSLink)
endif (VS_TOOL)

set(TOOLS_LIBS ${TOOLS_LIBS} PARENT_SCOPE)
set(TOOLS_BINS ${TOOLS_BINS} PARENT_SCOPE)



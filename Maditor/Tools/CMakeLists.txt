cmake_minimum_required (VERSION 2.6)

set(BLENDER_TOOL FALSE CACHE BOOL "Enables and installs blender tools")
set(VS_TOOL FALSE CACHE BOOL "Enables Visual Studio tools")


Project(Tools)


set (TOOLS_LIBS "")

if (BLENDER_TOOL)
endif (BLENDER_TOOL)

if (VS_TOOL)
  add_subdirectory(VSLink)
endif (VS_TOOL)

set(TOOLS_LIBS ${TOOLS_LIBS} PARENT_SCOPE)



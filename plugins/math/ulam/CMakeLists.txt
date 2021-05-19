cmake_minimum_required (VERSION 2.6)

Project(Ulam)

set(sources "")
FolderSources(sources "" "" ulam.cpp)

add_plugin(Ulam Madgine Math ${sources} PRECOMPILED_HEADER ulamlib.h SOURCE_ROOT .)

target_link_libraries(Ulam PUBLIC Modules)

set(sources "")
FolderSources(sources "" tools ulamtool.cpp heroeseditor.cpp)

add_tools(Ulam ${sources} PRECOMPILED_HEADER tools/ulamtoolslib.h SOURCE_ROOT tools)

target_link_plugins(UlamTools ClientTools)
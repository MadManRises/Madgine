cmake_minimum_required (VERSION 2.6)

Project(Generic)

set(sources)

FolderSources(sources "" "" commonlib.h genericlib.h streams.cpp)
	FolderSources(sources "" container refcounted_deque.cpp)

add_workspace_library(Generic ${sources} SOURCE_ROOT .. PRECOMPILED_HEADER genericlib.h)

install_to_workspace(Madgine TARGETS Generic EXPORT_LIB)

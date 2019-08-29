cmake_minimum_required (VERSION 2.6)

Project(Apis)

set(sources "")
FolderSources(sources "" src apisforward.h apislib.h)
	FolderSources(sources src im3d im3d.cpp im3d.h im3drenderpass.cpp im3drenderpass.h)

	

add_workspace_library(Apis ${sources} PRECOMPILED_HEADER src/apislib.h)


target_link_plugins(Apis OpenGL Client Base)

install_to_workspace(Apis TARGETS Apis EXPORT_LIB)
export_to_workspace(Apis)
Project(Filesystem)

set(sources filemanager.cpp filemanager.h)
	
add_workspace_library(Filesystem ${sources} SOURCE_ROOT ..)

target_link_libraries(Filesystem PUBLIC Modules)

install_to_workspace(Madgine EXPORT_LIB TARGETS Filesystem)
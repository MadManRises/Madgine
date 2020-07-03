Project(Ini)

set(sources iniformatter.cpp iniformatter.h)
	
add_workspace_library(Ini ${sources} SOURCE_ROOT ..)

target_link_libraries(Ini PUBLIC Modules)

install_to_workspace(Madgine EXPORT_LIB TARGETS Ini)
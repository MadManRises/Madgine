Project(Ini)

set(sources iniformatter.cpp iniformatter.h)
	
add_workspace_library(IniSerialize ${sources} SOURCE_ROOT ../..)

target_link_libraries(IniSerialize PUBLIC Meta)

install_to_workspace(Madgine EXPORT_LIB TARGETS IniSerialize)
Project(Memory)

set(sources memorybuffer.cpp memorybuffer.h memorymanager.cpp memorymanager.h)
	
add_workspace_library(Memory ${sources} SOURCE_ROOT ../..)

target_link_libraries(Memory PUBLIC Meta)

install_to_workspace(Madgine EXPORT_LIB TARGETS Memory)

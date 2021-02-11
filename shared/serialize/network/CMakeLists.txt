Project(Network)

set(sources networkbuffer.cpp networkbuffer.h networkmanager.cpp networkmanager.h)
	
add_workspace_library(Network ${sources} SOURCE_ROOT ../..)

target_link_libraries(Network PUBLIC Meta Modules)

install_to_workspace(Madgine EXPORT_LIB TARGETS Network)

if (BUILD_TESTING)
	add_subdirectory(test)
endif()
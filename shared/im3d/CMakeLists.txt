Project(ImGui)

set(sources im3d.cpp im3d.h im3d_internal.h)
	
add_workspace_library(Im3D ${sources} SOURCE_ROOT ..)

target_link_libraries(Im3D PUBLIC Modules ImGui FontLoader)

install_to_workspace(Shared EXPORT_LIB TARGETS Im3D)
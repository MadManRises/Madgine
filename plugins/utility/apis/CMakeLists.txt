cmake_minimum_required (VERSION 2.6)

Project(Apis)

set(sources "")
FolderSources(sources "" src apisforward.h apislib.h)
	FolderSources(sources src im3d im3d.cpp im3d.h)
	FolderSources(sources src imgui imgui.cpp imgui.h imguiaddons.cpp imguiaddons.h imgui_demo.cpp imgui_draw.cpp imgui_widgets.cpp imgui_internal.h)

	
add_workspace_library(Apis ${sources} PRECOMPILED_HEADER src/apislib.h)


target_link_libraries(Apis PUBLIC Modules)

install_to_workspace(Apis TARGETS Apis EXPORT_LIB)
export_to_workspace(Apis)


if (NOT ANDROID AND NOT EMSCRIPTEN)
	target_compile_definitions(Apis PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD)
endif()
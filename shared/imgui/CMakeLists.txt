
Project(ImGui)

set(sources imgui.cpp imgui.h imguiaddons.cpp imguiaddons.h imgui_demo.cpp imgui_draw.cpp imgui_widgets.cpp imgui_internal.h)
	
add_workspace_library(ImGui ${sources} SOURCE_ROOT ..)

target_link_libraries(ImGui PUBLIC Meta Modules)

install_to_workspace(Madgine EXPORT_LIB TARGETS ImGui)

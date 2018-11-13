
add_subdirectory(project)

add_subdirectory(plugins)

add_subdirectory(launcher)

export_to_workspace(Madgine)

cpack_add_component(Madgine REQUIRED)

cpack_add_component_group(Renderer EXPANDED)

configure_file(doc/Doxyfile.in Doxyfile)


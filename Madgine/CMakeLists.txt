
		set(CMAKE_MODULE_PATH
		  "${CMAKE_MODULE_PATH}"
		  "${CMAKE_CURRENT_SOURCE_DIR}/plugins/ogre/cmake"
		)     

find_package(OGRE REQUIRED)

add_subdirectory(project)

add_subdirectory(plugins)

export_to_workspace(Madgine)

cpack_add_component(Madgine REQUIRED)

cpack_add_component_group(Renderer EXPANDED)

configure_file(doc/Doxyfile.in Doxyfile)


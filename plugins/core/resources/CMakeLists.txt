
PROJECT(Resources)

set(sources "")
FolderSources(sources "" src)
	FolderSources(sources src Madgine)
		FolderSources(sources Madgine resources resourcebase.cpp resourcebase.h resourceloaderbase.cpp resourceloaderbase.h resourceloadercollector.cpp resourceloadercollector.h resourceloader.cpp resourceloader.h resourcemanager.cpp resourcemanager.h)

add_plugin(Resources Madgine Core ${sources})

target_link_plugins(Resources Root)

if (MSVC)
	target_link_options(Resources PUBLIC "/NATVIS:${CMAKE_CURRENT_LIST_DIR}/src/Resources.natvis")
endif()

add_subdirectory(loaders)

set(sources "")
FolderSources(sources "" tools)
	FolderSources(sources tools Madgine_Tools resourcestoolconfig.cpp resourcestoolconfig.h resourcestoolsforward.h resourcestoolslib.h)


add_tools(Resources ${sources} PRECOMPILED_HEADER tools/Madgine_Tools/resourcestoolslib.h)

target_link_plugins(ResourcesTools Tools AudioLoader MeshLoader)

if (MADGINE_CONFIGURATION)
	set(local_lists ${MADGINE_DATA_LISTS})
	list(FILTER MADGINE_DATA_LISTS EXCLUDE REGEX ".*resources\.list")
	list(FILTER local_lists INCLUDE REGEX ".*resources\.list")

	add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/resources.list
		COMMAND ./${MADGINE_TOOLING_BINARY} 
					-t
					-npc
					-lp ${PLUGIN_DEFINITION_FILE}
					--bake ${local_lists}
					--bake-output-list ${CMAKE_BINARY_DIR}/resources.list
		WORKING_DIRECTORY ${MADGINE_TOOLING_WORKING_DIRECTORY}
		DEPENDS MadgineTooling ${local_lists})
	add_custom_target(__unnamed_helper DEPENDS ${CMAKE_BINARY_DIR}/resources.list)

	list(APPEND MADGINE_DATA_LISTS ${CMAKE_BINARY_DIR}/resources.list)
	
	set(MADGINE_DATA_LISTS ${MADGINE_DATA_LISTS} CACHE INTERNAL "")
endif()
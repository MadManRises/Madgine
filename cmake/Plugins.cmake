include(Util)

include_guard()

include(Workspace)
include(ini)
include(Packaging)

set(PLUGIN_LIST "" CACHE INTERNAL "")
set(PROJECTS_LINKING_ALL_PLUGINS "" CACHE INTERNAL "")

if (STATIC_BUILD)

	if (NOT IS_ABSOLUTE ${STATIC_BUILD})
		set (STATIC_BUILD ${CMAKE_SOURCE_DIR}/${STATIC_BUILD})
	endif()

	if (NOT EXISTS ${STATIC_BUILD})
		MESSAGE(FATAL_ERROR "Config file ${STATIC_BUILD} not found!")
	endif()

	get_filename_component(extension ${STATIC_BUILD} EXT)
	if (NOT extension STREQUAL ".cfg")
		MESSAGE(FATAL_ERROR "Config file ${STATIC_BUILD} must have extension .cfg!")
	endif()

	get_filename_component(STATIC_BUILD_NAME ${STATIC_BUILD} NAME_WE)
	get_filename_component(STATIC_BUILD_DIR ${STATIC_BUILD} DIRECTORY)	

	function(get_static_config_file var name ext)
		set(${var} "${STATIC_BUILD_DIR}/${name}_${STATIC_BUILD_NAME}${ext}" PARENT_SCOPE)
	endfunction(get_static_config_file)

	read_ini_file(${STATIC_BUILD} PLUGINSELECTION)

	function(patch_toplevel_target target)
		get_target_property(target_flag ${target} PATCH_TOPLEVEL)
		if (NOT target_flag)
			set_target_properties(${target} PROPERTIES PATCH_TOPLEVEL TRUE)
			get_static_config_file(components_source components ".cpp")
			target_sources(${target} PRIVATE ${components_source})
		endif()
	endfunction(patch_toplevel_target)

endif ()


macro(add_plugin name base type)

	add_workspace_library(${name} ${ARGN})

	set_target_properties(${name} PROPERTIES OUTPUT_NAME Plugin_${base}_${type}_${name})

	collect_data(${name})

	set(installPlugin TRUE)

	if (NOT STATIC_BUILD)

		target_compile_definitions(${name} PRIVATE PLUGIN_BUILD)

	else()

		if (NOT PLUGINSELECTION_${type}_${name})
			MESSAGE (STATUS "Excluding Plugin '${name}' from ALL build.")
			set_target_properties(${name} PROPERTIES EXCLUDE_FROM_ALL TRUE)
			set(installPlugin FALSE)
		endif()

	endif()

	if (installPlugin)
		install_to_workspace(${name} TARGETS ${name} EXPORT_LIB)
		export_to_workspace(${name})

		cpack_add_component(${name} GROUP ${type})
	endif()

	set(PLUGIN_LIST ${PLUGIN_LIST} ${name} CACHE INTERNAL "")

	foreach(project ${PROJECTS_LINKING_ALL_PLUGINS})
		target_link_plugins(${project} ${name})
	endforeach()

endmacro(add_plugin)

function(target_link_plugins target)

	foreach(plugin ${ARGN})

		get_target_property(exclude ${plugin} EXCLUDE_FROM_ALL)
		if (NOT exclude)
			target_link_libraries(${target} PUBLIC ${plugin})
			#MESSAGE(STATUS "Linking ${plugin} to ${target}")
		else()
			#MESSAGE(STATUS "Not linking ${plugin} to ${target}")
		endif()

	endforeach()

	if (STATIC_BUILD)
		get_target_property(target_type ${target} TYPE)
		if (target_type STREQUAL "EXECUTABLE")
			patch_toplevel_target(${target})
		endif ()
	endif()

endfunction(target_link_plugins)


	
function(target_link_all_plugins target)
	
	set(available_core_libs Base)

	target_link_plugins(${target} ${PLUGIN_LIST} ${available_core_libs})

	set(PROJECTS_LINKING_ALL_PLUGINS ${PROJECTS_LINKING_ALL_PLUGINS} ${target} CACHE INTERNAL "")

endfunction()


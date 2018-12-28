include(Util)

include_guard()

include(Workspace)
include(ini)
include(Packaging)

set(PLUGIN_LIST "" CACHE INTERNAL "")

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

	function(add_executable name)
		_add_executable(${name} ${ARGN})
		
		get_static_config_file(components_source components ".cpp")
		target_sources(${name} PRIVATE ${components_source})

	endfunction(add_executable)


endif ()


macro(add_plugin name base type)

	add_workspace_library(${name} ${ARGN})

	set_target_properties(${name} PROPERTIES OUTPUT_NAME Plugin_${base}_${type}_${name})

	target_compile_definitions(${name} INTERFACE BUILD_PLUGIN_${name})

	collect_data(${name})

	if (NOT STATIC_BUILD)

		target_compile_definitions(${name} PRIVATE PLUGIN_BUILD)

		install_to_workspace(${name} TARGETS ${name} EXPORT_LIB)
		export_to_workspace(${name})

		cpack_add_component(${name} GROUP ${type})

		if (NOT MSVC)
			foreach (prop LINK_FLAGS LINK_FLAGS_DEBUG LINK_FLAGS_RELEASE LINK_FLAGS_RELWITHDEBINFO)
				get_target_property(flags ${name} ${prop})
				if(NOT flags)
					set(flags "")
				else()
					set(flags "${flags} ") # A space to cleanly separate from existing content
				endif()
				# Append our values
				set(flags "${flags}-rdynamic" )
				set_target_properties(${name} PROPERTIES ${prop} ${flags} )
			endforeach()
		endif()

	else()

		if (NOT PLUGINSELECTION_${type}_${name})
			MESSAGE (STATUS "Excluding Plugin '${name}' from ALL build.")
			set_target_properties(${name} PROPERTIES EXCLUDE_FROM_ALL TRUE)
		endif()

	endif()

	set(PLUGIN_LIST ${PLUGIN_LIST} ${name} CACHE INTERNAL "")

endmacro(add_plugin)

function(target_link_plugins target vis)

	foreach(plugin ${ARGN})

		get_target_property(exclude ${plugin} EXCLUDE_FROM_ALL)
		if (NOT exclude)
			target_link_libraries(${target} ${vis} ${plugin})
			#MESSAGE(STATUS "Linking ${plugin} to ${target}")
		else()
			#MESSAGE(STATUS "Not linking ${plugin} to ${target}")
		endif()

	endforeach()

endfunction(target_link_plugins)


	
function(target_link_all_plugins target vis)
	
	set(available_core_libs Base Client)

	target_link_plugins(${target} ${vis} ${PLUGIN_LIST} ${available_core_libs})

endfunction()

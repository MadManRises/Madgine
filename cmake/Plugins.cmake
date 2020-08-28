include(Util)

once()

include(Workspace)
include(ini)
include(Packaging)

set (PLUGIN_DEFINITION_FILE "" CACHE FILEPATH "Provide path to fixed plugin selection (optional)")

set(MODULES_ENABLE_PLUGINS ON CACHE INTERNAL "")
if (PLUGIN_DEFINITION_FILE)
	set(MODULES_ENABLE_PLUGINS OFF CACHE INTERNAL "")
	set(BUILD_SHARED_LIBS OFF CACHE BOOL "") #Provide default value OFF for given plugin config
else()
	set(BUILD_SHARED_LIBS ON CACHE BOOL "") #Provide default value ON for given plugin config
endif()

if (MODULES_ENABLE_PLUGINS AND NOT BUILD_SHARED_LIBS)
	MESSAGE(FATAL_ERROR "Currently static builds with plugins are not supported!")
endif()

set(PLUGIN_LIST "" CACHE INTERNAL "")
#set(PROJECTS_DEPENDING_ON_ALL_PLUGINS "" CACHE INTERNAL "")

if (NOT MODULES_ENABLE_PLUGINS)

	MESSAGE(STATUS ${PLUGIN_DEFINITION_FILE})

	if (NOT IS_ABSOLUTE ${PLUGIN_DEFINITION_FILE})
		set (PLUGIN_DEFINITION_FILE ${CMAKE_SOURCE_DIR}/${PLUGIN_DEFINITION_FILE})
	endif()

	if (NOT EXISTS ${PLUGIN_DEFINITION_FILE})
		MESSAGE(FATAL_ERROR "Config file ${PLUGIN_DEFINITION_FILE} not found! Please set PLUGIN_DEFINITION_FILE to a proper file if using MODULES_ENABLE_PLUGINS.")
	endif()

	get_filename_component(extension ${PLUGIN_DEFINITION_FILE} EXT)
	if (NOT extension STREQUAL ".cfg")
		MESSAGE(FATAL_ERROR "PLUGIN_DEFINITION_FILE ${PLUGIN_DEFINITION_FILE} must have extension .cfg!")
	endif()

	get_filename_component(PLUGIN_DEFINITION_NAME ${PLUGIN_DEFINITION_FILE} NAME_WE)
	get_filename_component(PLUGIN_DEFINITION_DIR ${PLUGIN_DEFINITION_FILE} DIRECTORY)	

	function(get_static_config_file var name ext)
		set(${var} "${PLUGIN_DEFINITION_DIR}/${name}_${PLUGIN_DEFINITION_NAME}${ext}" PARENT_SCOPE)
	endfunction(get_static_config_file)

	read_ini_file(${PLUGIN_DEFINITION_FILE} PLUGINSELECTION)

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

	set_target_properties(${name} PROPERTIES 
		OUTPUT_NAME Plugin_${base}_${type}_${name}
		PLUGIN_BASE ${base}
		PLUGIN_TYPE ${type})

	collect_data(${name})

	set(installPlugin TRUE)

	if (NOT MODULES_ENABLE_PLUGINS AND NOT PLUGINSELECTION_${type}_${name})		
		MESSAGE (STATUS "Excluding Plugin '${name}' from ALL build.")
		set_target_properties(${name} PROPERTIES EXCLUDE_FROM_ALL TRUE)
		set(installPlugin FALSE)
	else()
		set(PLUGIN_LIST ${PLUGIN_LIST} ${name} CACHE INTERNAL "")	
	endif()

	if (installPlugin)
		install_to_workspace(${name} TARGETS ${name} EXPORT_LIB)
		export_to_workspace(${name})

		cpack_add_component(${name} GROUP ${type})
	endif()
	
	#foreach(project ${PROJECTS_DEPENDING_ON_ALL_PLUGINS})
	#	target_link_plugins(${project} ${name})
	#endforeach()

endmacro(add_plugin)

function(target_link_plugins target)

	get_target_property(plugin_dependencies ${target} PLUGIN_DEPENDENCIES)
	if (NOT plugin_dependencies)
		set(plugin_dependencies)
	endif()

	foreach(plugin ${ARGN})

		get_target_property(exclude ${plugin} EXCLUDE_FROM_ALL)
		if (NOT exclude)
			target_link_libraries(${target} PUBLIC ${plugin})
			list(APPEND plugin_dependencies ${plugin})
			#MESSAGE(STATUS "Linking ${plugin} to ${target}")
		else()
			#MESSAGE(STATUS "Not linking ${plugin} to ${target}")
		endif()

	endforeach()

	if (plugin_dependencies)
		list(REMOVE_DUPLICATES plugin_dependencies)
		set_target_properties(${target} PROPERTIES PLUGIN_DEPENDENCIES "${plugin_dependencies}")
	endif()

	if (NOT MODULES_ENABLE_PLUGINS)
		get_target_property(target_type ${target} TYPE)
		if (target_type STREQUAL "EXECUTABLE")
			patch_toplevel_target(${target})
		endif ()
	endif()

endfunction(target_link_plugins)

function(target_link_plugin_groups target)

	get_target_property(plugin_group_dependencies ${target} PLUGIN_GROUP_DEPENDENCIES)
	if (NOT plugin_group_dependencies)
		set(plugin_group_dependencies)
	endif()

	list(APPEND plugin_group_dependencies ${ARGN})
	
	if (plugin_group_dependencies)
		list(REMOVE_DUPLICATES plugin_group_dependencies)
		set_target_properties(${target} PROPERTIES PLUGIN_GROUP_DEPENDENCIES "${plugin_group_dependencies}")
	endif()

endfunction(target_link_plugin_groups)
	
function(target_depend_on_all_plugins target)
	
	if (NOT MODULES_ENABLE_PLUGINS)
		target_link_plugins(${target} ${PLUGIN_LIST})
	else()
		add_dependencies(${target} ${PLUGIN_LIST})
	endif()

	MESSAGE(STATUS "Linking ${target} -> ${PLUGIN_LIST}")

	#set(PROJECTS_DEPENDING_ON_ALL_PLUGINS ${PROJECTS_DEPENDING_ON_ALL_PLUGINS} ${target} CACHE INTERNAL "")

endfunction()

function(plugin_group name)
	set(options EXCLUSIVE ATLEAST_ONE)
	set(oneValueArgs)
	set(multiValueArgs)
	cmake_parse_arguments(PLUGIN_GROUP "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})	

	if (PLUGIN_GROUP_UNPARSED_ARGUMENTS)
		MESSAGE(WARNING "Unrecognized options for plugin group: '${LIB_CONFIG_UNPARSED_ARGUMENTS}'")
	endif()

	if (PLUGIN_GROUP_EXCLUSIVE OR PLUGIN_GROUP_ATLEAST_ONE)
		get_target_property(plugin_group_definitions Modules PLUGIN_GROUP_DEFINITIONS)
		if (NOT plugin_group_definitions)
			set(plugin_group_definitions)
		endif()

		if (PLUGIN_GROUP_EXCLUSIVE)
			set(exclusive true)
		else()
			set(exclusive false)
		endif()

		if (PLUGIN_GROUP_ATLEAST_ONE)
			set(atleast_one true)
		else()
			set(atleast_one false)
		endif()

		list(APPEND plugin_group_definitions "setupSection(\"${name}\", ${exclusive}, ${atleast_one})")
	
		set_target_properties(Modules PROPERTIES PLUGIN_GROUP_DEFINITIONS "${plugin_group_definitions}")
	endif()
endfunction()
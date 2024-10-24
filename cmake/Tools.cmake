include(Util)

once()

include(Plugins)

macro(add_tools targetProject)

	set(options)
	set(oneValueArgs SOURCE_ROOT)
	set(multiValueArgs)
	cmake_parse_arguments(TOOL_CONFIG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})	

	get_target_property(base ${targetProject} PLUGIN_BASE)	

	if (NOT TOOL_CONFIG_SOURCE_ROOT)
		set(TOOL_CONFIG_SOURCE_ROOT tools)
	endif()

	add_plugin(${targetProject}Tools ${base} Tools ${TOOL_CONFIG_UNPARSED_ARGUMENTS} SOURCE_ROOT ${TOOL_CONFIG_SOURCE_ROOT} NO_DATA_COPY)

	target_link_plugins(${targetProject}Tools ${targetProject})

endmacro(add_tools)
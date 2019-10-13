include(Util)

once()

include(Plugins)

macro(add_tools targetProject)

	get_target_property(base ${targetProject} PLUGIN_BASE)	

	add_plugin(${targetProject}Tools ${base} Tools ${ARGN} SOURCE_ROOT tools)

	target_link_plugins(${targetProject}Tools ${targetProject})

endmacro(add_tools)
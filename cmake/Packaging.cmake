include(Util)

include_guard()

include(Workspace)

set (CMAKE_INSTALL_DEBUG_LIBRARIES TRUE)
include(InstallRequiredSystemLibraries)

include(GetPrerequisites)



macro(enable_packaging)

	if (EXISTS LICENSE.md)
		install(FILES LICENSE.md DESTINATION . RENAME LICENSE)
	endif()

	if (UNIX)
		set(CPACK_GENERATOR STGZ DEB)
		set(CPACK_DEBIAN_PACKAGE_MAINTAINER ${CPACK_PACKAGE_VENDOR})
	endif(UNIX)

	set(CPACK_PACKAGE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/packages)
	set(CPACK_PACKAGE_INSTALL_DIRECTORY ${PROJECT_NAME})
	set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.md)
	set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
	#set(CPACK_NSIS_PACKAGE_NAME ${PROJECT_NAME}-${PROJECT_VERSION})
	set(CPACK_NSIS_DISPLAY_NAME ${PROJECT_NAME})

	include(CPackComponent)
	include(CPack)
	

	function(install_to_workspace name)
		set(f_ARGN ARGN)
		_install_to_workspace("${name}" ${${f_ARGN}})
	
	endfunction(install_to_workspace)

endmacro()

add_custom_target(
    Remove_dependencies
    COMMAND ${CMAKE_COMMAND} -E echo "Removing old dependencies"
	COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/dependencies_bin"
)



function(collect_target_dependencies target)	

	add_dependencies(${target} Remove_dependencies)

	set(plugin_pattern ${ARGN})

	configure_file(${workspace_file_dir}/gatherDependencies.cmake.in ${CMAKE_BINARY_DIR}/gatherDependencies-${target}.cmake.in @ONLY)
	configure_file(${workspace_file_dir}/gatherDependencies-run.cmake.in ${CMAKE_BINARY_DIR}/gatherDependencies-run-${target}.cmake @ONLY)
	FILE(GENERATE OUTPUT ${CMAKE_BINARY_DIR}/gatherDependencies-${target}-$<CONFIG>.cmake INPUT ${CMAKE_BINARY_DIR}/gatherDependencies-${target}.cmake.in)

	add_custom_command(TARGET ${target} POST_BUILD COMMAND ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/gatherDependencies-run-${target}.cmake)

	install(DIRECTORY ${CMAKE_BINARY_DIR}/dependencies_bin/${target}/ DESTINATION bin COMPONENT ${target})

endfunction(collect_target_dependencies)

function(collect_custom_dependencies target name binary)

	add_custom_target(
		create_${name}_dep_folder
		DEPENDS Remove_dependencies
		COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/dependencies_bin/${name}"
	)

	add_dependencies(${target} create_${name}_dep_folder)
	
	GET_PREREQUISITES(${binary} Dependencies 1 1 "" "")

	get_filename_component(bin_path ${binary} DIRECTORY)

	foreach(dep ${Dependencies})
		GP_RESOLVE_ITEM(${binary} ${dep} "${bin_path}" "" path)
		GP_FILE_TYPE(${binary} ${path} type)
		if (NOT ${type} STREQUAL "local")
			add_custom_command(TARGET ${target} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${path} ${CMAKE_BINARY_DIR}/dependencies_bin/${name})
			#MESSAGE(STATUS ${dep} ${type} ${path})
		endif()
	endforeach()

endfunction(collect_custom_dependencies)
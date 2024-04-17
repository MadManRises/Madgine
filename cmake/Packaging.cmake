include(Util)

once()

include(Workspace)
include(binaryinfo)

set (CMAKE_INSTALL_DEBUG_LIBRARIES TRUE)
include(InstallRequiredSystemLibraries)

include(GetPrerequisites)

if (MADGINE_CONFIGURATION)
	file(GLOB lists "${MADGINE_CONFIGURATION}/*.list")
	set(MADGINE_DATA_LISTS "${lists}" CACHE INTERNAL "")	
endif ()

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

macro(collect_data target)

	if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/data)
		if (NOT MADGINE_CONFIGURATION AND (NOT BUILD_SHARED_LIBS OR MADGINE_FORCE_DATA_COLLECT))
			add_custom_target(
				${target}_copy_data ALL 				
				COMMAND ${CMAKE_COMMAND} -DSOURCE=${CMAKE_CURRENT_SOURCE_DIR}/data -DTARGET=${CMAKE_BINARY_DIR}/data -P ${workspace_file_dir}/util/flatcopy.cmake				
			)
		endif()
	endif()

endmacro()

macro(collect_data_lists)
	if (MADGINE_CONFIGURATION AND NOT TARGET copy_data)
		foreach (list ${MADGINE_DATA_LISTS})
			cmake_path(IS_PREFIX CMAKE_BINARY_DIR ${list} NORMALIZE is_generated)
			if (is_generated)
				set_property(SOURCE ${list} PROPERTY GENERATED 1)
			endif()
		endforeach()
		add_custom_target(
			copy_data ALL
			COMMAND ${CMAKE_COMMAND} "-DLISTS=\"${MADGINE_DATA_LISTS}\"" -DTARGET=${CMAKE_BINARY_DIR}/data -DBINARY_DIR=${CMAKE_BINARY_DIR} -P ${workspace_file_dir}/util/listcopy.cmake
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
			DEPENDS ${MADGINE_DATA_LISTS}
		)
	endif()
endmacro()
include(Util)

once()

cmake_policy(SET CMP0022 NEW)


include(CMakeDependentOption)
   
set(workspace_file_dir ${CMAKE_CURRENT_LIST_DIR} CACHE INTERNAL "")


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin CACHE INTERNAL "")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin CACHE INTERNAL "")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin CACHE INTERNAL "")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/bin CACHE INTERNAL "")

if (NOT WIN32)
	set (outDir ${CMAKE_BINARY_DIR}/bin)
	
	if (CMAKE_ANDROID_ARCH_ABI)
		set (outDir ${outDir}/${CMAKE_ANDROID_ARCH_ABI})
		set (BUILD_TESTING FALSE CACHE BOOL "" FORCE)
	endif()

	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${outDir})
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${outDir})
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${outDir})
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${outDir})

	set(CMAKE_INSTALL_RPATH $ORIGIN/)
	set(CMAKE_BUILD_RPATH $ORIGIN/)

endif()


get_property(support_shared GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS)

if (NOT support_shared)
	MESSAGE(STATUS "Forcing static libraries as shared libraries are not supported on that platform!")
	set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
endif()

if (NOT BUILD_SHARED_LIBS)
	add_definitions(-DSTATIC_BUILD=1)
endif()

add_definitions(-DBINARY_OUT_DIR="${CMAKE_BINARY_DIR}")

 
function(install_header name)    
	
	install(DIRECTORY src/ DESTINATION ${name}/include
      COMPONENT ${name}
	  FILES_MATCHING PATTERN "*.h"
    )	

endfunction(install_header)

function(install_to_workspace name)

	set(options EXPORT_LIB)
	set(oneValueArgs)
	set(multiValueArgs TARGETS)
	cmake_parse_arguments(OPTIONS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	if (OPTIONS_EXPORT_LIB)
		install(
			TARGETS ${OPTIONS_TARGETS}
			EXPORT ${name}
			RUNTIME DESTINATION bin COMPONENT ${name}
			LIBRARY DESTINATION lib COMPONENT ${name}
			BUNDLE DESTINATION App COMPONENT ${name}
			INCLUDES DESTINATION ${name}/include
			ARCHIVE DESTINATION ${name}/lib COMPONENT ${name}			
		)
	else()
		install(
			TARGETS ${OPTIONS_TARGETS} 
			EXPORT ${name}
			RUNTIME DESTINATION bin COMPONENT ${name}
			LIBRARY DESTINATION lib COMPONENT ${name}
			BUNDLE DESTINATION App COMPONENT ${name}
		)
	endif()

	foreach(target ${OPTIONS_TARGETS})
		get_target_property(TARGET_SOURCE_DIR ${target} SOURCE_DIR)
		if (EXISTS ${TARGET_SOURCE_DIR}/data)
			install(DIRECTORY ${TARGET_SOURCE_DIR}/data DESTINATION . COMPONENT ${name})
		endif()

		target_include_directories(${target} INTERFACE $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${target}/include>)
	endforeach()

endfunction(install_to_workspace)


function(export_to_workspace name)

	install_header(${name})

    install(
	    EXPORT ${name}
	    FILE ${name}Config.cmake
	    DESTINATION ${name}/lib/cmake/${name}
		COMPONENT ${name}
    )
    
endfunction(export_to_workspace)

#Customization-point for different platforms (e.g. Android)
macro(add_workspace_application target)

	add_executable(${target} ${ARGN})

endmacro(add_workspace_application)

macro(add_workspace_library name)

	set(options)
	set(oneValueArgs SOURCE_ROOT PRECOMPILED_HEADER)
	set(multiValueArgs)
	cmake_parse_arguments(LIB_CONFIG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})	
	
	add_library(${name} ${LIB_CONFIG_UNPARSED_ARGUMENTS})

	target_compile_definitions(${name} INTERFACE BUILD_${name})

	get_target_property(sourceDir ${name} SOURCE_DIR)

	if (NOT LIB_CONFIG_SOURCE_ROOT)
		set(LIB_CONFIG_SOURCE_ROOT src)
	endif()

	if (NOT EXISTS ${sourceDir}/${LIB_CONFIG_SOURCE_ROOT})
		MESSAGE(SEND_ERROR "Source directory '${sourceDir}/${LIB_CONFIG_SOURCE_ROOT}' does not exist for Library '${name}'. Use SOURCE_ROOT or create the directory.")
	endif()
	
	target_include_directories(${name} INTERFACE $<BUILD_INTERFACE:${sourceDir}/${LIB_CONFIG_SOURCE_ROOT}>)

	get_filename_component(abs_source_root ${LIB_CONFIG_SOURCE_ROOT} ABSOLUTE)
	set_target_properties(${name} PROPERTIES SOURCE_ROOT ${abs_source_root})

	if (LIB_CONFIG_PRECOMPILED_HEADER)
	
		get_filename_component(abs_precompile_include ${LIB_CONFIG_PRECOMPILED_HEADER} ABSOLUTE)

		target_precompile_headers(${name} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${abs_precompile_include}> )

		file(RELATIVE_PATH precompile_include ${abs_source_root} ${abs_precompile_include})

		set_target_properties(${name} PROPERTIES PRECOMPILED_HEADER ${precompile_include})

	endif()

	if (IWYU)
		set_property(TARGET ${name} PROPERTY CXX_INCLUDE_WHAT_YOU_USE
		"${IWYU};-Xiwyu;--pch_in_code;-Xiwyu;--prefix_header_includes=remove;-Xiwyu;--max_line_length=200;--driver-mode=cl")
	endif (IWYU)

endmacro(add_workspace_library)

macro(add_workspace_interface_library name)

	set(options)
	set(oneValueArgs SOURCE_ROOT PRECOMPILED_HEADER)
	set(multiValueArgs)
	cmake_parse_arguments(LIB_CONFIG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})	
	
	add_library(${name} INTERFACE ${LIB_CONFIG_UNPARSED_ARGUMENTS})	

	if (NOT LIB_CONFIG_SOURCE_ROOT)
		MESSAGE(SEND_ERROR "Source directory must be always set for Interface Library '${name}'. Use SOURCE_ROOT to specify it.")
	endif()
	
	target_include_directories(${name} INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/${LIB_CONFIG_SOURCE_ROOT}>)

endmacro(add_workspace_interface_library)


function(install_interface_to_workspace name)

	set(options)
	set(oneValueArgs SOURCE_ROOT)
	set(multiValueArgs TARGETS)
	cmake_parse_arguments(OPTIONS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

		install(
			TARGETS ${OPTIONS_TARGETS} 
			EXPORT ${name}
		)

	if (NOT OPTIONS_SOURCE_ROOT)
		MESSAGE(SEND_ERROR "Source directory must be always set for Interface Library installs. Use SOURCE_ROOT to specify it.")
	endif()

	foreach(target ${OPTIONS_TARGETS})
		
		get_filename_component(TARGET_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/${OPTIONS_SOURCE_ROOT} ABSOLUTE)

		if (EXISTS ${TARGET_SOURCE_DIR}/data)
			install(DIRECTORY ${TARGET_SOURCE_DIR}/data DESTINATION . COMPONENT ${name})
		endif()

		target_include_directories(${target} INTERFACE $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${target}/include>)
	endforeach()

endfunction(install_interface_to_workspace)


#Iterate over all files in platform

file(GLOB platforms "${CMAKE_CURRENT_LIST_DIR}/platform/*.cmake")

foreach(platform ${platforms})
	message(STATUS "Adding platform-code: ${platform}")
	include (${platform})
endforeach()

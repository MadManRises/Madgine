include(Util)

once()

cmake_policy(SET CMP0022 NEW)


include(CMakeDependentOption)
include(ExternalProject)

set (Workspace "" CACHE PATH "Path to the local Workspace")
   
set(workspace_file_dir ${CMAKE_CURRENT_LIST_DIR} CACHE INTERNAL "")


if (CMAKE_BUILD_TYPE STREQUAL "")
	message (FATAL_ERROR "No Build Type Specified!")
endif()

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
	set(BUILD_SHARED_LIBS OFF CACHE "" "" FORCE)
endif()

if (NOT BUILD_SHARED_LIBS)
	add_definitions(-DSTATIC_BUILD=1)
endif()

add_definitions(-DBINARY_OUT_DIR="${CMAKE_BINARY_DIR}")

if(MSVC)    
	# Set compiler options.
	set(variables
		CMAKE_C_FLAGS
		CMAKE_C_FLAGS_DEBUG
		CMAKE_C_FLAGS_MINSIZEREL
		CMAKE_C_FLAGS_RELEASE
		CMAKE_C_FLAGS_RELWITHDEBINFO
		CMAKE_CXX_FLAGS
		CMAKE_CXX_FLAGS_DEBUG
		CMAKE_CXX_FLAGS_MINSIZEREL
		CMAKE_CXX_FLAGS_RELEASE
		CMAKE_CXX_FLAGS_RELWITHDEBINFO
	)
	if(NOT BUILD_SHARED_LIBS)
		message(STATUS
		"MSVC -> forcing use of statically-linked runtime."
		)
		foreach(variable ${variables})
			if(${variable} MATCHES "/MD")
				string(REGEX REPLACE "/MD" "/MT" ${variable} "${${variable}}")
			endif()
		endforeach()
	else()
		message(STATUS
		"MSVC -> forcing use of dynamically-linked runtime."
		)
		foreach(variable ${variables})
			if(${variable} MATCHES "/MT")
				string(REGEX REPLACE "/MT" "/MD" ${variable} "${${variable}}")
			endif()
		endforeach()
	endif()
endif()

if (Workspace)
	set (CMAKE_MODULE_PATH
		"${CMAKE_MODULE_PATH}" "${Workspace}/cmake"
	)

	set (CMAKE_PREFIX_PATH
		"${CMAKE_PREFIX_PATH}" "${Workspace}"
	)

	if (NOT INSTALL_EXTERN)
		set(CMAKE_INSTALL_PREFIX ${Workspace})
	endif()
endif()

 
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
			INCLUDES DESTINATION ${name}/include
			ARCHIVE DESTINATION ${name}/lib COMPONENT ${name}			
		)
	else()
		install(
			TARGETS ${OPTIONS_TARGETS} 
			EXPORT ${name}
			RUNTIME DESTINATION bin COMPONENT ${name}
			LIBRARY DESTINATION lib COMPONENT ${name}
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

macro(find_workspace_package name)

	if (CONFIG_BUILD)
		set (__ARGS NO_SYSTEM_ENVIRONMENT_PATH)
	else()
		set (__ARGS )
	endif()

	find_package(${name} ${__ARGS} ${ARGN})

endmacro(find_workspace_package)

function(download_to_workspace lib url)


	if (Workspace STREQUAL "")
		message (FATAL_ERROR "Assertion failed: No Workspace set!")
	endif()   

	set(options)
	set(oneValueArgs GIT_TAG)
	set(multiValueArgs EXPORT_TARGETS DEFINITIONS)
	cmake_parse_arguments(LIB_CONFIG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	file(MAKE_DIRECTORY ${Workspace}/${lib}/download)
	
	if (NOT LIB_CONFIG_GIT_TAG)
		set (LIB_CONFIG_GIT_TAG master)
	endif ()
	
	set(third_party OFF)
	
	if (LIB_CONFIG_EXPORT_TARGETS)
		set (additional_args SOURCE_SUBDIR ..)
		
		configure_file(${workspace_file_dir}/thirdPartyInstall.in ${Workspace}/${lib}/download/src/CMakeLists.txt @ONLY)

		set(third_party ON)
	endif()

	find_workspace_package(${lib} CONFIG QUIET)

	if(NOT ${lib}_FOUND)

		message(STATUS "Begin Download ${lib}")

		if (CMAKE_GENERATOR_TOOLSET)
			set(toolset -T${CMAKE_GENERATOR_TOOLSET})
		endif()

		execute_process(
			COMMAND ${CMAKE_COMMAND} ${workspace_file_dir}/download 
				-G${CMAKE_GENERATOR}
				${toolset}
				-DWorkspace=${Workspace} 
				-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
				-DTHIRD_PARTY=${third_party}
				-Dlib=${lib}
				-Durl=${url}
				-DGIT_TAG=${LIB_CONFIG_GIT_TAG}
				-DDEFINITIONS=${LIB_CONFIG_DEFINITIONS}
			WORKING_DIRECTORY ${Workspace}/${lib}/download
			RESULT_VARIABLE retcode
		)
		if(NOT "${retcode}" STREQUAL "0")
			message(FATAL_ERROR "Fatal error setting config for '${lib}'")
		endif()

		execute_process(
			COMMAND ${CMAKE_COMMAND} --build .
			WORKING_DIRECTORY ${Workspace}/${lib}/download
			RESULT_VARIABLE retcode
		)
		if(NOT "${retcode}" STREQUAL "0")
			message(FATAL_ERROR "Fatal error building '${lib}'")
		endif()
	
		message(STATUS "End Download ${lib}")

	endif()

	ExternalProject_Add(Update_${lib}
	PREFIX ${Workspace}/${lib}/download
	CMAKE_GENERATOR ${CMAKE_GENERATOR}
	CMAKE_GENERATOR_TOOLSET ${CMAKE_GENERATOR_TOOLSET}
	GIT_REPOSITORY    ${url}
	GIT_TAG           ${LIB_CONFIG_GIT_TAG}
	INSTALL_DIR       ${Workspace}
	${additional_args}
	CMAKE_CACHE_ARGS
		-DCMAKE_INSTALL_PREFIX:PATH=${Workspace}
		-DWorkspace:PATH=${Workspace}
		-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
		-DCMAKE_TOOLCHAIN_FILE:FILEPATH=${CMAKE_TOOLCHAIN_FILE}
		${LIB_CONFIG_DEFINITIONS}
	)

	ExternalProject_Add_Step(Update_${lib} forceconfigure
		             COMMAND ${CMAKE_COMMAND} -E echo "Force configure of Update_${lib}"

					 DEPENDEES update
					 DEPENDERS configure
					 ALWAYS 1)

	set_target_properties(Update_${lib} PROPERTIES EXCLUDE_FROM_ALL TRUE)

	if (NOT TARGET Update_dependencies)
		add_custom_target(Update_dependencies)
	endif()
	add_dependencies(Update_dependencies Update_${lib})

endfunction(download_to_workspace)

macro(load_workspace_package lib)

	set(check ${ARGN})
	if (check)

		download_to_workspace(${lib} ${ARGN})
		
	endif()

	find_workspace_package(${lib} REQUIRED CONFIG)

endmacro(load_workspace_package)

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

endfunction(install_to_workspace)


#TODO Iterate over all files in subdirectory

include (android)
include (emscripten)
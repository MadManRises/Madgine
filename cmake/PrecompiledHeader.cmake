include (Util)

once()

function(add_precompiled_header target header)

	get_filename_component(name ${header} NAME_WE)

	set(output_path "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${target}_PCH.dir")
	if (GCC)
		set(output "${output_path}/${name}.gch")
	else()
		set(output "${output_path}/${name}.pch")
	#else()
	#	set(output "${output_path}/${name}.cpp${CMAKE_CXX_OUTPUT_EXTENSION}")
	endif()
    
	if (MSVC)
		set(pch_compile_flags "\"/Fp${output}\" /Yc${CMAKE_CURRENT_SOURCE_DIR}/${header} -Od")
		set(pch_use_flags "\"/Fp${output}\" /Yu${header}")
	elseif(GCC OR CLANG)
		set(pch_compile_flags "-x c++-header -O0")
		set(pch_use_flags "-Winvalid-pch")
		if (CLANG)
			set(pch_use_flags "${pch_use_flags} -include-pch ${output}")
		else()
			set(pch_use_flags "${pch_use_flags} -I${output_path}")
		endif()
	endif()

	set(cppsource "${CMAKE_CURRENT_BINARY_DIR}/${name}.cpp")	
	file(GENERATE OUTPUT "${cppsource}" CONTENT "#include \"${CMAKE_CURRENT_SOURCE_DIR}/${header}\"")
	set_source_files_properties(
		${cppsource}
		PROPERTIES
		LANGUAGE CXX
		COMPILE_FLAGS ${pch_compile_flags}
		OBJECT_OUTPUTS ${output}
	)
	add_library(${target}_PCH OBJECT ${cppsource})
	set_target_properties(${target}_PCH PROPERTIES EXCLUDE_FROM_ALL TRUE)
	add_dependencies(${target} ${target}_PCH)

	target_include_directories(${target}_PCH PRIVATE $<TARGET_PROPERTY:${target},INCLUDE_DIRECTORIES>)	

	get_property(sources TARGET ${target} PROPERTY SOURCES)
	foreach(source ${sources})
		if(source MATCHES \\.\(cc|cxx|cpp|c\)$)
			get_source_file_property(compile_flags "${source}" COMPILE_FLAGS)
			if (NOT compile_flags)
				set(compile_flags)
			endif()
			get_source_file_property(object_depends "${source}" OBJECT_DEPENDS)
			if(NOT object_depends)
				set(object_depends)
			endif()
			if(source MATCHES \\.\(cc|cxx|cpp\)$)
				list(APPEND object_depends "${output}")
				set_source_files_properties(${source} PROPERTIES				
					COMPILE_FLAGS "${compile_flags} ${flags}" #remove this when COMPILE_LANGUAGE is supported on test server
				)	
			endif()
			set_source_files_properties(${source} PROPERTIES
				OBJECT_DEPENDS "${object_depends}"
				#COMPILE_FLAGS "${compile_flags} $<$<COMPILE_LANGUAGE:CXX>:${flags}>"
			)	
		endif()
    endforeach()

endfunction()

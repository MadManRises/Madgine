include(Util)

include_guard()

set(my_dir ${CMAKE_CURRENT_LIST_DIR})

macro(generate_binary_info target)

	if (NOT STATIC_BUILD)

		target_sources(${target} PRIVATE ${my_dir}/binaryinfo.cpp)

		set (major_version ${PROJECT_VERSION_MAJOR})
		if (NOT major_version)
			set(major_version 1)
		endif()

		set (minor_version ${PROJECT_VERSION_MINOR})
		if (NOT minor_version)
			set(minor_version 0)
		endif()

		set (patch_number ${PROJECT_VERSION_PATCH})
		if (NOT patch_number)
			set(patch_number 0)
		endif()

		set_source_files_properties(${my_dir}/binaryinfo.cpp PROPERTIES
		  COMPILE_DEFINITIONS "BINARY_SOURCE_ROOT=\"${PROJECT_SOURCE_DIR}\";BINARY_BINARY_DIR=\"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}\";BINARY_MAJOR_VERSION=${major_version};BINARY_MINOR_VERSION=${minor_version};BINARY_PATCH_NUMBER=${patch_number};BINARY_FILENAME=\"$<TARGET_FILE_NAME:${target}>\""
		)

	endif()

endmacro()
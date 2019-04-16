include(Util)

include_guard()

set(binaryinfo_dir ${CMAKE_CURRENT_LIST_DIR} CACHE INTERNAL "")

macro(generate_binary_info target)

	if (NOT STATIC_BUILD)

		target_sources(${target} PRIVATE ${binaryinfo_dir}/binaryinfo.cpp)
		target_link_libraries(${target} PUBLIC Interfaces)

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

		
		target_compile_definitions(${target} PRIVATE 
		PROJECT_NAME=${target}
		BINARY_PROJECT_ROOT=\"${PROJECT_SOURCE_DIR}\"
		BINARY_BINARY_DIR=\"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}\"
		BINARY_MAJOR_VERSION=${major_version}
		BINARY_MINOR_VERSION=${minor_version}
		BINARY_PATCH_NUMBER=${patch_number}
		BINARY_FILENAME=\"$<TARGET_FILE_NAME:${target}>\"
		PRECOMPILED_HEADER_PATH=\"$<TARGET_PROPERTY:${target},PRECOMPILED_HEADER>\"
		BINARY_SOURCE_ROOT=\"$<TARGET_PROPERTY:${target},SOURCE_ROOT>\")


	endif()

endmacro()
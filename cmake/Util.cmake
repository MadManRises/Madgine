
if (NOT COMMAND include_guard)

	macro(include_guard)

		get_filename_component(guard_name ${CMAKE_CURRENT_LIST_FILE} NAME_WE)

		if (__${guard_name}_GUARD__)
			return()
		endif()
		set(__${guard_name}_GUARD__ TRUE)

	endmacro(include_guard)

endif()

include_guard()

  
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	add_compile_options(-Wall -fpermissive)
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")
endif ()

if (NOT MSVC)
	add_compile_options(-fvisibility=hidden)
endif()

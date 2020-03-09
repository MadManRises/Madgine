include(Util)

once()

cmake_policy(SET CMP0003 NEW)
cmake_policy(SET CMP0004 NEW)

set(emscripten_file_dir ${CMAKE_CURRENT_LIST_DIR} CACHE INTERNAL "")

if (EMSCRIPTEN)

	set(CMAKE_SHARED_LIBRARY_SUFFIX ".wasm")

	include(Plugins)

	function (add_workspace_application target)

		add_executable(${target} ${ARGN})

		set_target_properties(${target} PROPERTIES SUFFIX ".html")

		_target_link_libraries(${target} PRIVATE "--preload-file ${CMAKE_BINARY_DIR}/data@/data")

	endfunction (add_workspace_application)

	function(target_link_libraries target vis)
		set(args ${ARGN})
		if (NOT vis MATCHES "INTERFACE|PUBLIC|PRIVATE")
			set(args ${vis} ${args})
			set(vis PUBLIC)
		endif()

		foreach(lib ${args})
			if (TARGET ${lib})
				get_target_property(type ${lib} TYPE)

				if (type STREQUAL "SHARED_LIBRARY")
					target_include_directories(${target} ${vis} $<TARGET_PROPERTY:${lib},INTERFACE_INCLUDE_DIRECTORIES>)
					add_dependencies(${target} ${lib})
				else()
					_target_link_libraries(${target} ${vis} ${lib})
				endif()
			else()
				_target_link_libraries(${target} ${vis} ${lib})
			endif()
		endforeach()
	endfunction(target_link_libraries)


endif()
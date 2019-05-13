include(Util)

once()

if (EMSCRIPTEN)
	set(CMAKE_SHARED_LIBRARY_SUFFIX ".wasm")

	function (add_workspace_application target)

		add_executable(${target} ${ARGN})

		set_target_properties(${target} PROPERTIES SUFFIX ".html")

		target_link_libraries(${target} PRIVATE "--preload-file data")

	endfunction (add_workspace_application)

endif()
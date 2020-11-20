include (Util)

once()

include (Workspace)

set (MEMORYCHECK_COMMAND_OPTIONS "--xml=yes --xml-file=memchecks/%p.memcheck")

include (CTest)

if (BUILD_TESTING)

	enable_testing()

	add_subdirectory(test/googletest)

	function(add_test_impl name)
	
		add_executable(${name} ${ARGN})

		target_link_libraries(${name} PRIVATE ${CMAKE_THREAD_LIBS_INIT})

		set (framework )

		if (EMSCRIPTEN)
			set (framework "node")
		endif()

		if (NOT ANDROID)
			add_test(
				NAME ${name}
				COMMAND ${framework} $<TARGET_FILE:${name}> --gtest_output=xml:${name}.xml -npc
				WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
			)
		endif()

		if (EMSCRIPTEN)
			target_link_libraries(${name} PRIVATE "nodefs.js")
		endif()

	endfunction()

	function(add_gtest name)
		add_test_impl(${name} ${ARGN})
	    target_link_libraries(${name} PRIVATE Interfaces_gtest_main gtest)
	endfunction(add_gtest)

	function(add_gtestmock name)
		add_test_impl(${name} ${ARGN})
		target_link_libraries(${name} PRIVATE Interfaces_gmock_main gmock gtest)
	endfunction(add_gtestmock)

	function(add_gmock name)
		add_test_impl(${name} ${ARGN})
		target_link_libraries(${name} PRIVATE Interfaces_gmock_main gmock)
	endfunction(add_gmock)

endif()


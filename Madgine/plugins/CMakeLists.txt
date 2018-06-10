
include_directories(${Madgine_SOURCE_DIR}/Madgine/src)

include(Plugins)

file(COPY ${CMAKE_CURRENT_LIST_DIR}/gatherDependencies.cmake.in DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_LIST_DIR}/gatherDependencies-run.cmake.in DESTINATION ${CMAKE_BINARY_DIR})

function(collect_dependencies target)	

	set(plugin_pattern ${ARGN})

	configure_file(${CMAKE_BINARY_DIR}/gatherDependencies.cmake.in ${CMAKE_BINARY_DIR}/gatherDependencies-${target}.cmake.in @ONLY)
	configure_file(${CMAKE_BINARY_DIR}/gatherDependencies-run.cmake.in ${CMAKE_BINARY_DIR}/gatherDependencies-run-${target}.cmake @ONLY)
	FILE(GENERATE OUTPUT ${CMAKE_BINARY_DIR}/gatherDependencies-${target}-$<CONFIG>.cmake INPUT ${CMAKE_BINARY_DIR}/gatherDependencies-${target}.cmake.in)

	add_custom_command(TARGET ${target} POST_BUILD COMMAND ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/gatherDependencies-run-${target}.cmake)

	install(DIRECTORY ${CMAKE_BINARY_DIR}/dependencies_bin/${target}/ DESTINATION bin COMPONENT ${target})

endfunction(collect_dependencies)



add_subdirectory(ogre)


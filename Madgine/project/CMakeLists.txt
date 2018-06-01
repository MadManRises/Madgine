
add_subdirectory(interfaces)
add_subdirectory(base)
if (BUILD_CLIENT AND NOT CONFIG_BUILD)
	add_subdirectory(client)
endif ()
add_subdirectory(server)
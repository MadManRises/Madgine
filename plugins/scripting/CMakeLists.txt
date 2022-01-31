project(Scripting)

if (CMAKE_BUILD_TYPE MATCHES Debug)
	add_subdirectory(python3)
endif()
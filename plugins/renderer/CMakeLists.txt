
add_subdirectory(opengl)

if (WINDOWS)
	add_subdirectory(directx11)
	add_subdirectory(directx12)
endif ()

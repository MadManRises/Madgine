
add_subdirectory(opengl)

if (WINDOWS)
	add_subdirectory(directx11)
	if (NOT MINGW)
		add_subdirectory(directx12)
	endif()
endif ()

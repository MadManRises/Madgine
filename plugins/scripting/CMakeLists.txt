project(Scripting)

if (NOT ANDROID AND NOT EMSCRIPTEN)
	add_subdirectory(python3)
endif()
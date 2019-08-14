if (ANDROID)

	add_subdirectory(android)

elseif (EMSCRIPTEN)

	add_subdirectory(emscripten)

else()

	add_subdirectory(ois)

endif()

option(PLUGINS_BUILD_SDLINPUT "Build the SDL-input plugin")
if (PLUGINS_BUILD_SDLINPUT)
	add_subdirectory(sdl)
endif()
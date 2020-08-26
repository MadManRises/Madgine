if (ANDROID)

	add_subdirectory(android)

elseif (EMSCRIPTEN)

	add_subdirectory(emscripten)

else()

	add_subdirectory(ois)

endif()

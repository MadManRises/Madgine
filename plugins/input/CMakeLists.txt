if (ANDROID)

	add_subdirectory(android)

else()

	option(OIS_BUILD_SHARED_LIBS "Build shared libraries" OFF)
	option(OIS_BUILD_DEMOS "Build demo applications" OFF)

	add_subdirectory(ois)

endif()
Project(GooglePlayServices)

if (NOT TARGET PlayServices)
	return()
endif()



MESSAGE(STATUS "--- Google Play ---")

if (NOT GooglePlay_Config)
	MESSAGE(STATUS "No GooglePlay_Config provided. Plugin disabled. Set 'GooglePlay_Config' to the location of the games-ids.xml file downloaded from Google Play Console in your toplevel CMakeLists file.")
	MESSAGE(STATUS "--- Google Play End ---")
	return()
endif ()

#MESSAGE(STATUS "Creating steam_appid.txt for Debug build.")
#file(WRITE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/steam_appid.txt ${Steam_AppId})

#MESSAGE(STATUS "AppId: ${Steam_AppId}")

#MESSAGE(STATUS "--- Steam End ---")




set(sources "")
FolderSources(sources "" src)
FolderSources(sources src Madgine)
	FolderSources(sources Madgine firstparty googleplayservices.cpp googleplayservices.h)

add_plugin(GooglePlayServices Madgine FirstParty ${sources} PRECOMPILED_HEADER src/Madgine/googleplayserviceslib.h)

target_link_plugins(GooglePlayServices FirstParty)

target_link_libraries(GooglePlayServices PRIVATE PlayServices)

	configure_file(${GooglePlay_Config} res/values/games-ids.xml)
	list(APPEND ANDROID_RESOURCES
		${CMAKE_CURRENT_BINARY_DIR}/res/
	)
	set(ANDROID_RESOURCES "${ANDROID_RESOURCES}" CACHE INTERNAL "")



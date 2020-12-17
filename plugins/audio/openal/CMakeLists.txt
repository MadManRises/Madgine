cmake_minimum_required (VERSION 2.6)

Project(OpenAL)



set(sources "")
FolderSources(sources "" src)
FolderSources(sources src OpenAL openalforward.h openallib.h openalaudiocontext.cpp openalaudiocontext.h)
	FolderSources(sources OpenAL util)

add_plugin(OpenALPlugin Madgine Audio ${sources} PRECOMPILED_HEADER src/OpenAL/openallib.h)

target_link_plugins(OpenALPlugin Scene AudioLoader)

target_link_libraries(OpenALPlugin PRIVATE OpenAL)
#target_include_directories(OpenALPlugin PRIVATE ${OPENGL_INCLUDE_DIR})


set(sources "")
FolderSources(sources "" tools)
	FolderSources(sources tools OpenAL_Tools openaltoolsforward.h openaltoolslib.h)


add_tools(OpenALPlugin ${sources} PRECOMPILED_HEADER tools/OpenAL_Tools/openaltoolslib.h)

target_link_plugins(OpenALPluginTools Tools)
target_link_libraries(OpenALPluginTools PRIVATE OpenAL)
#target_include_directories(OpenALPluginTools PRIVATE ${OPENGL_INCLUDE_DIR})

if (BUILD_TESTING)
	add_subdirectory(test)
endif()

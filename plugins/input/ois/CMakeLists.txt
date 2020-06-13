
cmake_policy(SET CMP0049 OLD)
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0022 NEW)

Project(OIS)



set(sources "")
FolderSources(sources "" src oisinputhandler.cpp oisinputhandler.h)


add_plugin(OISHandler Madgine Input ${sources} PRECOMPILED_HEADER src/oislib.h)

target_link_plugins(OISHandler Client)

target_link_libraries(OISHandler PRIVATE OIS)

if (BUILD_TESTING)
	add_subdirectory(test)
endif()
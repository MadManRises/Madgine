option(OIS_BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(OIS_BUILD_DEMOS "Build demo applications" OFF)

add_subdirectory(ois)

if (BUILD_SHARED_LIBS)
	set_property(TARGET OIS PROPERTY POSITION_INDEPENDENT_CODE ON)
endif()


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
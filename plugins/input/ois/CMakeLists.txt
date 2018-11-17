option(OIS_BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(OIS_BUILD_DEMOS "Build demo applications" OFF)

add_subdirectory(ois)


cmake_policy(SET CMP0049 OLD)
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0022 NEW)

Project(OIS)



set(sources "")
FolderSources(sources "" src oisinputhandler.cpp oisinputhandler.h)


add_plugin(OISHandler Madgine Input ${sources})

#add_precompiled_header(OpenGL src/OpenGL/opengllib.h FORCEINCLUDE)



target_link_libraries(OISHandler PUBLIC Client PRIVATE OIS)

if (BUILD_TESTING)
	add_subdirectory(test)
endif()
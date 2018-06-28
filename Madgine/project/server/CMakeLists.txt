cmake_minimum_required (VERSION 2.6)

cmake_policy(SET CMP0049 OLD)
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0022 NEW)

Project(Server)

find_package(Threads REQUIRED)

set(sources "")
FolderSources(sources "" "../../src")
FolderSources(sources "../../src" Madgine serverforward.h serverlib.h)
	FolderSources(sources Madgine app serverapplication.cpp serverapplication.h serverappsettings.h servertimer.cpp servertimer.h)
	FolderSources(sources Madgine scene serverscenemanager.cpp serverscenemanager.h)
		FolderSources(sources scene entity)
			FolderSources(sources entity components serveranimation.cpp serveranimation.h servermesh.cpp servermesh.h serverskeletonvisualizer.cpp serverskeletonvisualizer.h servertransform.cpp servertransform.h)
	FolderSources(sources Madgine scripting)
	FolderSources(sources Madgine server serverbase.cpp serverbase.h serverappinstance.cpp serverappinstance.h)
	FolderSources(sources Madgine util serverlog.cpp serverlog.h)


add_library(MadgineServer SHARED ${sources} serverlib.cpp)

add_precompiled_header(MadgineServer ../../src/Madgine/serverlib.h FORCEINCLUDE SOURCE_CXX serverlib.cpp)

target_link_libraries(MadgineServer PUBLIC Base)

target_link_libraries(MadgineServer PRIVATE ${CMAKE_THREAD_LIBS_INIT})

target_compile_definitions(MadgineServer INTERFACE MADGINE_SERVER_BUILD)


if (UNIX)
	target_link_libraries (MadgineServer PRIVATE stdc++fs)
endif (UNIX)
 
if (BUILD_TESTING)
	add_subdirectory(test)
endif()


install_to_workspace(Madgine TARGETS MadgineServer EXPORT_LIB)

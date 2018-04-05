cmake_minimum_required (VERSION 2.6)

cmake_policy(SET CMP0049 OLD)
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0022 NEW)

Project(Server)

set(sources "")
FolderSources(sources "" "" serverlib.cpp)
FolderSources(sources "" src serverforward.h serverlib.h)
	FolderSources(sources src app serverapplication.cpp serverapplication.h serverappsettings.h servertimer.cpp servertimer.h)
	FolderSources(sources src scene serverscenemanager.cpp serverscenemanager.h)
		FolderSources(sources scene entity)
			FolderSources(sources entity components serveranimation.cpp serveranimation.h servermesh.cpp servermesh.h serverskeletonvisualizer.cpp serverskeletonvisualizer.h servertransform.cpp servertransform.h)
	FolderSources(sources src scripting)
		FolderSources(sources scripting parsing serverscriptparser.cpp serverscriptparser.h)
	FolderSources(sources src server serverbase.cpp serverbase.h serverappinstance.cpp serverappinstance.h)
	FolderSources(sources src util serverlog.cpp serverlog.h)


add_library(MadgineServer SHARED ${sources} serverlib.cpp)

target_include_directories(MadgineServer PUBLIC $<BUILD_INTERFACE:${Server_SOURCE_DIR}/src>)

add_precompiled_header(MadgineServer src/serverlib.h FORCEINCLUDE SOURCE_CXX serverlib.cpp)

target_link_libraries(MadgineServer PRIVATE Base Interfaces ${CMAKE_THREAD_LIBS_INIT})

target_compile_definitions(MadgineServer INTERFACE MADGINE_SERVER_BUILD)


if (CMAKE_COMPILER_IS_GNUCXX)
	target_link_libraries (MadgineServer PRIVATE stdc++fs)
endif (CMAKE_COMPILER_IS_GNUCXX)
 
if (BUILD_TESTING)
	add_subdirectory(test)
endif()


install_to_workspace(Madgine MadgineServer)

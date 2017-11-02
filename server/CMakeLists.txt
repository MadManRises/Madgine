cmake_minimum_required (VERSION 2.6)

cmake_policy(SET CMP0011 OLD)
cmake_policy(SET CMP0049 OLD)
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0022 NEW)

Project(SERVER)

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


include_directories(${SERVER_SOURCE_DIR}/src)

add_library(MadgineServer SHARED ${sources} serverlib.cpp)

add_precompiled_header(MadgineServer src/serverlib.h FORCEINCLUDE SOURCE_CXX serverlib.cpp)

include_targets(MadgineServer Base Interfaces)

target_compile_definitions(MadgineServer INTERFACE MADGINE_SERVER_BUILD)

target_link_libraries(MadgineServer PRIVATE ${CMAKE_THREAD_LIBS_INIT})
 
if (BUILD_TESTS)
	add_subdirectory(test)
endif(BUILD_TESTS)


installToWorkspace(MadgineServer Madgine)

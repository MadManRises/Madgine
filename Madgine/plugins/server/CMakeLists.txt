cmake_minimum_required (VERSION 2.6)

cmake_policy(SET CMP0049 OLD)
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0022 NEW)

Project(Server)

find_package(Threads REQUIRED)

set(sources "")
FolderSources(sources "" src)
FolderSources(sources src MadgineServer serverforward.h serverlib.h)
	FolderSources(sources MadgineServer scene serverlightmanager.cpp serverlightmanager.h)
		FolderSources(sources scene entity)
			FolderSources(sources entity components serveranimation.cpp serveranimation.h servermesh.cpp servermesh.h serverskeletonvisualizer.cpp serverskeletonvisualizer.h servertransform.cpp servertransform.h)
	FolderSources(sources MadgineServer server serverframeloop.cpp serverframeloop.h servertimer.cpp servertimer.h)


add_plugin(MadgineServer Madgine Renderer ${sources})

#add_precompiled_header(MadgineServer src/MadgineServer/serverlib.h FORCEINCLUDE SOURCE_CXX serverlib.cpp)

target_link_libraries(MadgineServer PUBLIC Base)

target_link_libraries(MadgineServer PRIVATE ${CMAKE_THREAD_LIBS_INIT})


if (UNIX)
	target_link_libraries (MadgineServer PRIVATE stdc++fs)
endif (UNIX)
 
if (BUILD_TESTING)
	add_subdirectory(test)
endif()


install_to_workspace(MadgineServer TARGETS MadgineServer EXPORT_LIB)
export_to_workspace(MadgineServer)

cpack_add_component(MadgineServer DISPLAY_NAME Server GROUP Renderer)

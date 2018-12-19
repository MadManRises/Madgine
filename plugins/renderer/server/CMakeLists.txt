cmake_minimum_required (VERSION 2.6)

cmake_policy(SET CMP0049 OLD)
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0022 NEW)

Project(Server)

find_package(Threads REQUIRED)

set(sources "")
FolderSources(sources "" src)
FolderSources(sources src Server serverforward.h serverlib.h)
	FolderSources(sources Server scene serverlightmanager.cpp serverlightmanager.h)
		FolderSources(sources scene entity)
			FolderSources(sources entity components serveranimation.cpp serveranimation.h servermesh.cpp servermesh.h serverskeletonvisualizer.cpp serverskeletonvisualizer.h servertransform.cpp servertransform.h)
	FolderSources(sources Server server servertimer.cpp servertimer.h)


add_plugin(Server Madgine Renderer ${sources} PRECOMPILED_HEADER src/Server/serverlib.h)


target_link_libraries(Server PUBLIC Base)

target_link_libraries(Server PRIVATE ${CMAKE_THREAD_LIBS_INIT})


if (UNIX)
	target_link_libraries (Server PRIVATE stdc++fs)
endif (UNIX)
 
if (BUILD_TESTING)
	add_subdirectory(test)
endif()

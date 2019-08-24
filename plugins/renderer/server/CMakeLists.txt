cmake_minimum_required (VERSION 2.6)

Project(ServerRenderer)

set(sources "")
FolderSources(sources "" src)
FolderSources(sources src Server serverforward.h serverlib.h)
	FolderSources(sources Server scene serverlightmanager.cpp serverlightmanager.h)
		FolderSources(sources scene entity)
			FolderSources(sources entity components serveranimation.cpp serveranimation.h servermesh.cpp servermesh.h serverskeletonvisualizer.cpp serverskeletonvisualizer.h)


add_plugin(ServerRenderer Madgine Renderer ${sources} PRECOMPILED_HEADER src/Server/serverlib.h)


target_link_plugins(ServerRenderer Base)

if (INTERFACES_ENABLE_THREADING)
	find_package(Threads REQUIRED)
	target_link_libraries(ServerRenderer PRIVATE ${CMAKE_THREAD_LIBS_INIT})
endif()

 
if (BUILD_TESTING)
	add_subdirectory(test)
endif()

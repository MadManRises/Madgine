cmake_minimum_required (VERSION 2.6)

Project(Base)

set(sources "")
FolderSources(sources "" src)
	FolderSources(sources src Madgine baseforward.h baselib.h)
		FolderSources(sources Madgine app application.cpp application.h appsettings.h globalapibase.cpp globalapibase.h globalapicollector.cpp globalapicollector.h)
		FolderSources(sources Madgine core keyvalueregistry.cpp keyvalueregistry.h root.cpp root.h)

add_plugin(Base Madgine Core ${sources} PRECOMPILED_HEADER src/Madgine/baselib.h)

target_link_plugins(Base Resources)

target_link_libraries(Base PUBLIC Modules)

target_link_libraries(Base PRIVATE ${CMAKE_THREAD_LIBS_INIT})


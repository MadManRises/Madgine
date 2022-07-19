Project(Base)

set(sources "")
FolderSources(sources "" src)
	FolderSources(sources src Madgine baseforward.h baselib.h)
		FolderSources(sources Madgine base application.cpp application.h globalapibase.cpp globalapibase.h globalapicollector.cpp globalapicollector.h keyvalueregistry.cpp keyvalueregistry.h root.cpp root.h)

add_plugin(Base Madgine Core ${sources} PRECOMPILED_HEADER src/Madgine/baselib.h)

target_link_libraries(Base PUBLIC CLI CodeGen Modules)

target_link_libraries(Base PRIVATE ${CMAKE_THREAD_LIBS_INIT})


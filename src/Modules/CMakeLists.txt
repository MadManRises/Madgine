cmake_minimum_required (VERSION 2.6)

Project(Modules)


set(sources)

FolderSources(sources "" "" modulesforward.h moduleslib.h)
	FolderSources(sources "" debug)
		FolderSources(sources debug profiler processstats.cpp processstats.h profile.h profiler.cpp profiler.h profilerthread.cpp profilerthread.h staticprocess.cpp staticprocess.h)
if (MODULES_ENABLE_PLUGINS)
	FolderSources(sources "" plugins pluginmanager.cpp pluginmanager.h pluginsection.cpp pluginsection.h plugin.cpp plugin.h)
endif()
	FolderSources(sources "" threading barrier.cpp barrier.h connection.cpp connection.h connectionstore.cpp connectionstore.h datamutex.cpp datamutex.h global.h globalstorage.h scheduler.cpp scheduler.h signal.h signalrouter.h signalstub.cpp signalstub.h task.h taskqueue.cpp taskqueue.h tasknode.h threadlocal.h threadstorage.cpp threadstorage.h workgroup.cpp workgroup.h workgrouphandle.h workgroupstorage.cpp workgroupstorage.h)
	FolderSources(sources "" uniquecomponent indexholder.h uniquecomponent.h uniquecomponentcollector.h uniquecomponentcontainer.h uniquecomponentcollectormanager.cpp uniquecomponentcollectormanager.h uniquecomponentregistry.cpp uniquecomponentregistry.h)
	

add_workspace_library(Modules ${sources} SOURCE_ROOT .. PRECOMPILED_HEADER moduleslib.h)

configure_file(modulesconfig.in.h modulesconfig.h)
target_include_directories(Modules PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)

target_link_libraries(Modules PUBLIC Interfaces CodeGen CLI Ini)

if (INTERFACES_ENABLE_THREADING)
	find_package(Threads REQUIRED)
	target_link_libraries (Modules PRIVATE ${CMAKE_THREAD_LIBS_INIT})
endif()



generate_binary_info(Modules)

install_to_workspace(Madgine TARGETS Modules EXPORT_LIB)

target_compile_definitions(Modules PRIVATE 
		PLUGIN_GROUP_DEFINITIONS=$<JOIN:$<TARGET_PROPERTY:Modules,PLUGIN_GROUP_DEFINITIONS>,,>)

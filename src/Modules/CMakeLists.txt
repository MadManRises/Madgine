Project(Modules VERSION 1.0.0)


option(MODULES_ENABLE_TASK_TRACKING "" OFF)

set(sources)

FolderSources(sources "" "" modulesforward.h moduleslib.h)
	FolderSources(sources "" debug)
		FolderSources(sources debug profiler processstats.cpp processstats.h profile.h profiler.cpp profiler.h profilerthread.cpp profilerthread.h staticprocess.cpp staticprocess.h)
if (MODULES_ENABLE_TASK_TRACKING)
		FolderSources(sources debug tasktracking tasktracker.cpp tasktracker.h)
endif()
if (MODULES_ENABLE_PLUGINS)
	FolderSources(sources "" plugins pluginmanager.cpp pluginmanager.h pluginsection.cpp pluginsection.h plugin.cpp plugin.h)
endif()
	FolderSources(sources "" threading connection.cpp connection.h connectionstore.cpp connectionstore.h datamutex.cpp datamutex.h global.h globalstorage.h scheduler.cpp scheduler.h signal.h signalrouter.h signalstub.cpp signalstub.h task.h taskhandle.cpp taskhandle.h taskpromise.cpp taskpromise.h taskqueue.cpp taskqueue.h threadlocal.h threadstorage.cpp threadstorage.h workgroup.cpp workgroup.h workgrouphandle.h workgroupstorage.cpp workgroupstorage.h)
	FolderSources(sources "" uniquecomponent indexholder.h uniquecomponent.h uniquecomponentcollector.h uniquecomponentcontainer.h uniquecomponentcollectormanager.cpp uniquecomponentcollectormanager.h uniquecomponentregistry.cpp uniquecomponentregistry.h)
	

add_workspace_library(Modules ${sources} SOURCE_ROOT .. PRECOMPILED_HEADER moduleslib.h)

configure_file(modulesconfig.in.h modulesconfig.h)
target_include_directories(Modules PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)

target_link_libraries(Modules PUBLIC Interfaces Ini)

if (WINDOWS)
	if (NOT BUILD_SHARED_LIBS AND MINGW)
		target_link_libraries (Modules PRIVATE -Wl,-Bstatic,--whole-archive pthread -Wl,-Bdynamic,--no-whole-archive)
	else()
		find_package(Threads)
		target_link_libraries (Modules PRIVATE ${CMAKE_THREAD_LIBS_INIT})
	endif()
endif()



generate_binary_info(Modules)

install_to_workspace(Madgine TARGETS Modules EXPORT_LIB)

target_compile_definitions(Modules PRIVATE 
		PLUGIN_GROUP_DEFINITIONS=$<JOIN:$<TARGET_PROPERTY:Modules,PLUGIN_GROUP_DEFINITIONS>,,>)

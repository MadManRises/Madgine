cmake_minimum_required (VERSION 2.6)

Project(Modules)


set(sources)

FolderSources(sources "" "" modulesforward.h moduleslib.h)
	FolderSources(sources "" cli cli.cpp cli.h)
	FolderSources(sources "" debug)
		FolderSources(sources debug profiler processstats.cpp processstats.h profiler.cpp profiler.h)
	FolderSources(sources "" generic templates.h)
	FolderSources(sources "" ini inifile.cpp inifile.h iniformatter.cpp iniformatter.h)
	FolderSources(sources "" keyvalue accessor.h apimethod.h boundapimethod.cpp boundapimethod.h container_traits.h keyvalue.h keyvalueiterate.h metatable.cpp metatable.h nativeobject.cpp nativeobject.h objectfieldaccessor.cpp objectfieldaccessor.h objectptr.cpp objectptr.h scopebase.h typedscopeptr.cpp typedscopeptr.h valuetype.cpp valuetype.h valuetypeexception.h)
	FolderSources(sources "" math atlas2.cpp atlas2.h bounds.cpp bounds.h geometry3.cpp geometry3.h matrix4.cpp matrix4.h matrix3.cpp matrix3.h plane.h quaternion.h ray.h vector4.h vector3.h vector2.h)
	FolderSources(sources "" network networkbuffer.cpp networkbuffer.h networkmanager.cpp networkmanager.h)
if (MODULES_ENABLE_PLUGINS)
	FolderSources(sources "" plugins pluginmanager.cpp pluginmanager.h pluginsection.cpp pluginsection.h plugin.cpp plugin.h)
endif()
	FolderSources(sources "" serialize fileexception.h formatter.cpp formatter.h syncable.cpp syncable.h serializable.cpp serializable.h serializableids.h serializableunit.cpp serializableunit.h serializeexception.h serializemanager.cpp serializemanager.h toplevelserializableunit.cpp toplevelserializableunit.h serializetable.cpp serializetable.h)
		FolderSources(sources serialize container action.h creationhelper.h synced.h syncablecontainer.h serializablecontainer.h serialized.h tupleserialize.h unithelper.cpp unithelper.h)
		FolderSources(sources serialize streams buffered_streambuf.cpp buffered_streambuf.h bufferedstream.cpp bufferedstream.h filebuffer.cpp filebuffer.h serializestream.cpp serializestream.h)
			FolderSources(sources streams debugging streamdebugging.cpp streamdebugging.h)
	FolderSources(sources "" signalslot connection.cpp connection.h connectionstore.cpp connectionstore.h slot.h signal.h signalrouter.h signalstub.cpp signalstub.h slot.cpp slot.h task.h taskqueue.cpp taskqueue.h)
	FolderSources(sources "" threading datamutex.cpp datamutex.h defaulttaskqueue.cpp defaulttaskqueue.h scheduler.cpp scheduler.h tasknode.h  threadlocal.cpp threadlocal.h workgroup.cpp workgroup.h workgrouphandle.h)
	FolderSources(sources "" uniquecomponent indexholder.h uniquecomponent.h uniquecomponentcollector.h uniquecomponentcontainer.h uniquecomponentcollectormanager.cpp uniquecomponentcollectormanager.h uniquecomponentregistry.cpp uniquecomponentregistry.h)
	FolderSources(sources "" util log.cpp log.h standardlog.cpp standardlog.h utilmethods.cpp utilmethods.h)
	

add_workspace_library(Modules ${sources} SOURCE_ROOT .. PRECOMPILED_HEADER moduleslib.h)

configure_file(modulesconfig.in.h modulesconfig.h)
target_include_directories(Interfaces PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)

target_link_libraries(Modules PUBLIC Interfaces)

if (INTERFACES_ENABLE_THREADING)
	find_package(Threads REQUIRED)
	target_link_libraries (Modules PRIVATE ${CMAKE_THREAD_LIBS_INIT})
endif()

if (ANDROID)
	target_link_libraries(Modules PRIVATE log)	
endif()


generate_binary_info(Modules)

install_to_workspace(Madgine TARGETS Modules EXPORT_LIB)

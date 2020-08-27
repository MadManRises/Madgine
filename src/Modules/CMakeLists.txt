cmake_minimum_required (VERSION 2.6)

Project(Modules)


set(sources)

FolderSources(sources "" "" modulesforward.h moduleslib.h)
	FolderSources(sources "" cli cli.cpp cli.h parameter.h parameterbase.cpp parameterbase.h)
	FolderSources(sources "" debug)
		FolderSources(sources debug profiler processstats.cpp processstats.h profile.h profiler.cpp profiler.h profilerthread.cpp profilerthread.h staticprocess.cpp staticprocess.h)
		FolderSources(sources "" ini inifile.cpp inifile.h inisection.cpp inisection.h)
	FolderSources(sources "" keyvalue accessor.h apifunction.cpp apifunction.h boundapifunction.cpp boundapifunction.h functiontable.cpp functiontable.h keyvalue.h keyvalueiterator.h keyvaluevirtualiterator.h metatable.cpp metatable.h nativeobject.cpp nativeobject.h objectfieldaccessor.cpp objectfieldaccessor.h objectptr.cpp objectptr.h scopebase.h scopefield.cpp scopefield.h scopeiterator.cpp scopeiterator.h typedscopeptr.cpp typedscopeptr.h valuetype.cpp valuetype.h valuetype_desc.cpp valuetype_desc.h valuetype_forward.cpp valuetype_forward.h valuetype_types.h valuetypeexception.h)
	FolderSources(sources "" keyvalueutil keyvalueregistry.cpp keyvalueregistry.h valuetypeserialize.cpp valuetypeserialize.h)
	FolderSources(sources "" madgineobject madgineobject.cpp madgineobject.h madgineobjectobserver.cpp madgineobjectobserver.h objectstate.h)
	FolderSources(sources "" math atlas2.cpp atlas2.h bounds.cpp bounds.h geometry3.cpp geometry3.h matrix4.cpp matrix4.h matrix3.cpp matrix3.h plane.h quaternion.cpp quaternion.h ray.h transformation.cpp transformation.h vector4.h vector3.h vector2.h)
if (MODULES_ENABLE_PLUGINS)
	FolderSources(sources "" plugins pluginmanager.cpp pluginmanager.h pluginsection.cpp pluginsection.h plugin.cpp plugin.h)
endif()
	FolderSources(sources "" resources resourcebase.cpp resourcebase.h resourceloaderbase.cpp resourceloaderbase.h resourceloadercollector.cpp resourceloadercollector.h resourceloader.cpp resourceloader.h resourcemanager.cpp resourcemanager.h)
	FolderSources(sources "" serialize creationhelper.h fileexception.h formatter.cpp formatter.h syncable.cpp syncable.h serializable.h serializableids.h serializableunit.cpp serializableunit.h serializeexception.h serializemanager.cpp serializemanager.h syncmanager.cpp syncmanager.h toplevelserializableunit.cpp toplevelserializableunit.h serializetable.cpp serializetable.h unithelper.cpp unithelper.h)
		FolderSources(sources serialize container action.h synced.h syncablecontainer.h serializablecontainer.h serialized.h)
		FolderSources(sources serialize formatter safebinaryformatter.cpp safebinaryformatter.h)
		FolderSources(sources serialize streams buffered_streambuf.cpp buffered_streambuf.h bufferedstream.cpp bufferedstream.h serializestream.cpp serializestream.h serializestreambuf.cpp serializestreambuf.h wrappingserializestreambuf.cpp wrappingserializestreambuf.h)
	FolderSources(sources "" threading barrier.cpp barrier.h connection.cpp connection.h connectionstore.cpp connectionstore.h datamutex.cpp datamutex.h defaulttaskqueue.cpp defaulttaskqueue.h global.h globalstorage.h scheduler.cpp scheduler.h signal.h signalrouter.h signalstub.cpp signalstub.h slot.cpp slot.h task.h taskqueue.cpp taskqueue.h tasknode.h threadlocal.h threadstorage.cpp threadstorage.h workgroup.cpp workgroup.h workgrouphandle.h workgroupstorage.cpp workgroupstorage.h)
	FolderSources(sources "" uniquecomponent indexholder.h uniquecomponent.h uniquecomponentcollector.h uniquecomponentcontainer.h uniquecomponentcollectormanager.cpp uniquecomponentcollectormanager.h uniquecomponentregistry.cpp uniquecomponentregistry.h)
	

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

target_compile_definitions(Modules PRIVATE 
		PLUGIN_GROUP_DEFINITIONS=$<JOIN:$<TARGET_PROPERTY:Modules,PLUGIN_GROUP_DEFINITIONS>,,>)
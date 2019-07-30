cmake_minimum_required (VERSION 2.6)

Project(Modules)


set(sources)

FolderSources(sources "" "" modulesforward.h moduleslib.h)
	FolderSources(sources "" cli cli.cpp cli.h)
	FolderSources(sources "" debug)
		FolderSources(sources debug profiler processstats.cpp processstats.h profiler.cpp profiler.h)
	FolderSources(sources "" generic templates.h)
	FolderSources(sources "" ini inifile.cpp inifile.h)
	FolderSources(sources "" keyvalue accessor.h apimethod.h boundapimethod.cpp boundapimethod.h container_traits.h invscopeptr.h keyvalue.h keyvalueiterate.h metatable.cpp metatable.h scopebase.h typedscopeptr.cpp typedscopeptr.h typenamekeyimpl.h valuetype.cpp valuetype.h valuetypeexception.h)
	FolderSources(sources "" lua lapi.c lcode.c lctype.c ldebug.c ldo.c ldump.c lfunc.c lgc.c llex.c lmem.c lobject.c lopcodes.c lparser.c lstate.c lstring.c ltable.c ltm.c lundump.c lutf8lib.c lvm.c lzio.c lauxlib.c lbaselib.c lbitlib.c lcorolib.c ldblib.c liolib.c lmathlib.c loslib.c lstrlib.c ltablib.c loadlib.c linit.c)
	FolderSources(sources "" math bounds.cpp bounds.h matrix4.cpp matrix4.h matrix3.cpp matrix3.h vector4.cpp vector4.h vector3.cpp vector3.h vector2.cpp vector2.h)
	FolderSources(sources "" network networkbuffer.cpp networkbuffer.h networkmanager.cpp networkmanager.h)
	FolderSources(sources "" os osapi.h osapi_windows.cpp)
if (NOT STATIC_BUILD)
	FolderSources(sources "" plugins pluginmanager.cpp pluginmanager.h pluginsection.cpp pluginsection.h plugin.cpp plugin.h)
endif()
	FolderSources(sources "" scripting scriptingexception.h)
		FolderSources(sources scripting datatypes argumentlist.cpp argumentlist.h intluatableinstance.cpp intluatableinstance.h luatable.cpp luatable.h luatablefieldaccessor.cpp luatablefieldaccessor.h luatableinstance.cpp luatableinstance.h luatableiterator.cpp luatableiterator.h luathread.h stringluatableinstance.cpp stringluatableinstance.h)
		FolderSources(sources scripting parsing parseexception.h scriptparser.cpp scriptparser.h)
		FolderSources(sources scripting types apihelper.cpp apihelper.h globalscopebase.cpp globalscopebase.h luastate.cpp luastate.h scopeentity.h)
	FolderSources(sources "" serialize fileexception.h observable.cpp observable.h serializable.cpp serializable.h serializableids.h serializableunit.cpp serializableunit.h serializeexception.h serializemanager.cpp serializemanager.h toplevelserializableunit.cpp toplevelserializableunit.h)
		FolderSources(sources serialize container action.h container.h creationhelper.h list.h map.h observed.h observablecontainer.h serializablecontainer.h serialized.h set.h sortedcontainer.h tupleserialize.h unithelper.cpp unithelper.h unsortedcontainer.h vector.h)
		FolderSources(sources serialize streams buffered_streambuf.cpp buffered_streambuf.h bufferedstream.cpp bufferedstream.h filebuffer.cpp filebuffer.h serializestream.cpp serializestream.h)
			FolderSources(sources streams debugging streamdebugging.cpp streamdebugging.h)
	FolderSources(sources "" signalslot connection.cpp connection.h connectionstore.cpp connectionstore.h slot.h signal.h signalrouter.h signalstub.cpp signalstub.h slot.cpp slot.h task.h taskqueue.cpp taskqueue.h)
	FolderSources(sources "" threading datamutex.cpp datamutex.h defaulttaskqueue.cpp defaulttaskqueue.h framelistener.h frameloop.cpp frameloop.h scheduler.cpp scheduler.h tasknode.h  threadlocal.cpp threadlocal.h workgroup.cpp workgroup.h workgrouphandle.h)
	FolderSources(sources "" uniquecomponent indexholder.h uniquecomponent.h uniquecomponentcollector.h uniquecomponentcontainer.h uniquecomponentcollectormanager.cpp uniquecomponentcollectormanager.h uniquecomponentregistry.cpp uniquecomponentregistry.h)
	FolderSources(sources "" util log.cpp log.h standardlog.cpp standardlog.h utilmethods.cpp utilmethods.h)
	

add_workspace_library(Modules ${sources} SOURCE_ROOT .. PRECOMPILED_HEADER moduleslib.h)

target_link_libraries(Modules PUBLIC Interfaces)

find_package(Threads REQUIRED)
target_link_libraries (Modules PRIVATE ${CMAKE_THREAD_LIBS_INIT})

if (ANDROID)
	target_link_libraries(Modules PRIVATE log)	
endif()

install_to_workspace(Madgine TARGETS Modules EXPORT_LIB)

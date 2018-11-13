cmake_minimum_required (VERSION 2.6)

cmake_policy(SET CMP0049 OLD)
cmake_policy(SET CMP0048 NEW)

Project(Interfaces)

find_package(Threads REQUIRED)

set(sources)
FolderSources(sources "" "../../src")
FolderSources(sources "../../src" Interfaces interfacesforward.h interfaceslib.h)
	FolderSources(sources Interfaces database exceptionmessages.h message.h messages.h)
	FolderSources(sources Interfaces debug stacktrace.cpp stacktrace.h traceback.cpp traceback.h)
		FolderSources(sources debug memory memory.cpp memory.h memoryglobal.h statsmemoryresource.cpp statsmemoryresource.h untrackedmemoryresource.cpp untrackedmemoryresource.h)
		FolderSources(sources debug profiler processstats.cpp processstats.h profiler.cpp profiler.h)
	FolderSources(sources Interfaces generic keyvalue.h keyvalueiterate.cpp keyvalueiterate.h singleton.h templates.h valuetype.cpp valuetype.h)
	FolderSources(sources Interfaces ini inifile.cpp inifile.h)
	FolderSources(sources Interfaces math bounds.cpp bounds.h matrix3.cpp matrix3.h vector3.cpp vector3.h vector2.cpp vector2.h)
	FolderSources(sources Interfaces network linuxapi.cpp networkbuffer.cpp networkbuffer.h networkmanager.cpp networkmanager.h networkstream.cpp networkstream.h socketapi.cpp socketapi.h winapi.cpp)
	FolderSources(sources Interfaces plugins pluginmanager.cpp pluginmanager.h plugin.cpp plugin.h)
	FolderSources(sources Interfaces scripting scriptingexception.h)
		FolderSources(sources scripting datatypes argumentlist.cpp argumentlist.h intluatableinstance.cpp intluatableinstance.h luatable.cpp luatable.h luatablefieldaccessor.cpp luatablefieldaccessor.h luatableinstance.cpp luatableinstance.h luatableiterator.cpp luatableiterator.h luathread.h stringluatableinstance.cpp stringluatableinstance.h)
		FolderSources(sources scripting parsing parseexception.h scriptparser.cpp scriptparser.h)
		FolderSources(sources scripting types api.cpp api.h apihelper.cpp apihelper.h globalscopebase.cpp globalscopebase.h luastate.cpp luastate.h mapper.h scopebase.cpp scopebase.h scope_cast.h scope.h)
	FolderSources(sources Interfaces serialize fileexception.h observable.cpp observable.h serializable.cpp serializable.h serializableids.h serializableunit.cpp serializableunit.h serializeexception.h serializemanager.cpp serializemanager.h toplevelserializableunit.cpp toplevelserializableunit.h)
		FolderSources(sources serialize container action.h container.h creationhelper.h list.h map.h observed.h observablecontainer.h serializablecontainer.h serialized.h set.h sortedcontainer.h tupleserialize.h unithelper.cpp unithelper.h unsortedcontainer.h vector.h)
		FolderSources(sources serialize streams buffered_streambuf.cpp buffered_streambuf.h bufferedstream.cpp bufferedstream.h filebuffer.cpp filebuffer.h serializestream.cpp serializestream.h)
			FolderSources(sources streams debugging streamdebugging.cpp streamdebugging.h)
	FolderSources(sources Interfaces signalslot connection.cpp connection.h connectionmanager.cpp connectionmanager.h connectionstore.cpp connectionstore.h slot.h signal.h signalrouter.h)
	FolderSources(sources Interfaces util exception.cpp exception.h log.cpp log.h process.cpp process.h standardlog.cpp standardlog.h utilmethods.cpp utilmethods.h)
	FolderSources(sources Interfaces window windowapi.h windowapi_windows.cpp windoweventlistener.h)
FolderSources(sources "" vstools Madgine.natvis)

add_workspace_library(Interfaces  ${LIBRARY_TYPE} ${sources} SOURCE_ROOT ../..)

add_precompiled_header(Interfaces ../../src/Interfaces/interfaceslib.h FORCEINCLUDE)

target_link_libraries (Interfaces PRIVATE lualib ${CMAKE_THREAD_LIBS_INIT})

if (WIN32)
	target_link_libraries (Interfaces PRIVATE ws2_32 DbgHelp)	
endif()

if (UNIX)
	target_link_libraries (Interfaces PRIVATE stdc++fs dl)
endif (UNIX)
 
if (BUILD_TESTING)
	add_subdirectory(test)	
endif()

install_to_workspace(Madgine TARGETS Interfaces EXPORT_LIB)

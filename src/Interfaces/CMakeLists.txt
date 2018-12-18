cmake_minimum_required (VERSION 2.6)

cmake_policy(SET CMP0049 OLD)
cmake_policy(SET CMP0048 NEW)

Project(Interfaces)

find_package(Threads REQUIRED)

set(sources)

FolderSources(sources "" "" commonlib.h interfacesforward.h interfaceslib.h)
	FolderSources(sources "" database exceptionmessages.h message.h messages.h)
	FolderSources(sources "" debug stacktrace.cpp stacktrace.h traceback.cpp traceback.h)
		FolderSources(sources debug memory memory.cpp memory.h memoryglobal.h statsmemoryresource.cpp statsmemoryresource.h untrackedmemoryresource.cpp untrackedmemoryresource.h)
		FolderSources(sources debug profiler processstats.cpp processstats.h profiler.cpp profiler.h)
	FolderSources(sources "" generic keyvalue.h keyvalueiterate.cpp keyvalueiterate.h singleton.h templates.h valuetype.cpp valuetype.h)
	FolderSources(sources "" ini inifile.cpp inifile.h)
	FolderSources(sources "" lua lapi.c lcode.c lctype.c ldebug.c ldo.c ldump.c lfunc.c lgc.c llex.c lmem.c lobject.c lopcodes.c lparser.c lstate.c lstring.c ltable.c ltm.c lundump.c lutf8lib.c lvm.c lzio.c lauxlib.c lbaselib.c lbitlib.c lcorolib.c ldblib.c liolib.c lmathlib.c loslib.c lstrlib.c ltablib.c loadlib.c linit.c)
	FolderSources(sources "" math bounds.cpp bounds.h matrix4.cpp matrix4.h matrix3.cpp matrix3.h vector3.cpp vector3.h vector2.cpp vector2.h)
	FolderSources(sources "" network linuxapi.cpp networkbuffer.cpp networkbuffer.h networkmanager.cpp networkmanager.h networkstream.cpp networkstream.h socketapi.cpp socketapi.h winapi.cpp)
if (NOT STATIC_BUILD)
	FolderSources(sources "" plugins pluginmanager.cpp pluginmanager.h pluginsection.cpp pluginsection.h plugin.cpp plugin.h)
endif()
	FolderSources(sources "" scripting scriptingexception.h)
		FolderSources(sources scripting datatypes argumentlist.cpp argumentlist.h intluatableinstance.cpp intluatableinstance.h luatable.cpp luatable.h luatablefieldaccessor.cpp luatablefieldaccessor.h luatableinstance.cpp luatableinstance.h luatableiterator.cpp luatableiterator.h luathread.h stringluatableinstance.cpp stringluatableinstance.h)
		FolderSources(sources scripting parsing parseexception.h scriptparser.cpp scriptparser.h)
		FolderSources(sources scripting types api.cpp api.h apihelper.cpp apihelper.h globalscopebase.cpp globalscopebase.h luastate.cpp luastate.h mapper.h scopebase.cpp scopebase.h scope_cast.h scope.h)
	FolderSources(sources "" serialize fileexception.h observable.cpp observable.h serializable.cpp serializable.h serializableids.h serializableunit.cpp serializableunit.h serializeexception.h serializemanager.cpp serializemanager.h toplevelserializableunit.cpp toplevelserializableunit.h)
		FolderSources(sources serialize container action.h container.h creationhelper.h list.h map.h observed.h observablecontainer.h serializablecontainer.h serialized.h set.h sortedcontainer.h tupleserialize.h unithelper.cpp unithelper.h unsortedcontainer.h vector.h)
		FolderSources(sources serialize streams buffered_streambuf.cpp buffered_streambuf.h bufferedstream.cpp bufferedstream.h filebuffer.cpp filebuffer.h serializestream.cpp serializestream.h)
			FolderSources(sources streams debugging streamdebugging.cpp streamdebugging.h)
	FolderSources(sources "" signalslot connection.cpp connection.h connectionmanager.cpp connectionmanager.h connectionstore.cpp connectionstore.h slot.h signal.h signalrouter.h)
	FolderSources(sources "" util exception.cpp exception.h log.cpp log.h process.cpp process.h runtime.cpp runtime.h standardlog.cpp standardlog.h utilmethods.cpp utilmethods.h)
	FolderSources(sources "" window windowapi.h windowapi_linux.cpp windowapi_windows.cpp windoweventlistener.h)

add_workspace_library(Interfaces ${sources} ${CMAKE_SOURCE_DIR}/misc/Madgine.natvis SOURCE_ROOT ..)

add_precompiled_header(Interfaces interfaceslib.h FORCEINCLUDE)


if (NOT STATIC_BUILD)
	target_compile_definitions(Interfaces PRIVATE SHARED_LIB_SUFFIX="${CMAKE_SHARED_LIBRARY_SUFFIX}" SHARED_LIB_PREFIX="${CMAKE_SHARED_LIBRARY_PREFIX}")
endif()


target_link_libraries (Interfaces PRIVATE ${CMAKE_THREAD_LIBS_INIT})

if (WIN32)
	target_link_libraries (Interfaces PRIVATE ws2_32 DbgHelp)	
endif()

if (UNIX)
	find_package(X11 REQUIRED)
	find_package(OpenGL REQUIRED)
	target_link_libraries (Interfaces PRIVATE stdc++fs dl X11 OpenGL::GLX)
endif (UNIX)

install_to_workspace(Madgine TARGETS Interfaces EXPORT_LIB)

cmake_minimum_required (VERSION 2.6)

cmake_policy(SET CMP0049 OLD)
cmake_policy(SET CMP0048 NEW)

Project(Interfaces)

find_package(Threads REQUIRED)

set(sources interfaceslib.cpp)
FolderSources(sources "" "../../src")
FolderSources(sources "../../src" Madgine interfacesforward.h interfaceslib.h)
	FolderSources(sources Madgine generic exceptionmessages.h keyvalue.h keyvalueiterate.cpp keyvalueiterate.h message.h singleton.h templates.h valuetype.cpp valuetype.h)
	FolderSources(sources Madgine math vector3.cpp vector3.h vector2.cpp vector2.h)
	FolderSources(sources Madgine network linuxapi.cpp networkbuffer.cpp networkbuffer.h networkmanager.cpp networkmanager.h networkstream.cpp networkstream.h socketapi.cpp socketapi.h winapi.cpp)
	FolderSources(sources Madgine plugins pluginmanager.cpp pluginmanager.h plugin.cpp plugin.h)
	FolderSources(sources Madgine scripting scriptingexception.h)
		FolderSources(sources scripting datatypes argumentlist.cpp argumentlist.h intluatableinstance.cpp intluatableinstance.h luatable.cpp luatable.h luatablefieldaccessor.cpp luatablefieldaccessor.h luatableinstance.cpp luatableinstance.h luatableiterator.cpp luatableiterator.h luathread.h stringluatableinstance.cpp stringluatableinstance.h)
		FolderSources(sources scripting parsing parseexception.h scriptparser.cpp scriptparser.h)
		FolderSources(sources scripting types api.cpp api.h apihelper.cpp apihelper.h luastate.cpp luastate.h mapper.h scopebase.cpp scopebase.h scope_cast.h scope.h)
	FolderSources(sources Madgine serialize fileexception.h observable.cpp observable.h serializable.cpp serializable.h serializableunit.cpp serializableunit.h serializeexception.h serializehelper.cpp serializehelper.h serializemanager.cpp serializemanager.h toplevelserializableunit.cpp toplevelserializableunit.h)
		FolderSources(sources serialize container action.h container.h creationhelper.h list.h map.h observed.h observablecontainer.h serializablecontainer.h serialized.h set.h sortedcontainer.h unithelper.cpp unithelper.h unsortedcontainer.h vector.h)
		FolderSources(sources serialize streams buffered_streambuf.cpp buffered_streambuf.h bufferedstream.cpp bufferedstream.h filebuffer.cpp filebuffer.h serializestream.cpp serializestream.h)
			FolderSources(sources streams debugging streamdebugging.cpp streamdebugging.h)
	FolderSources(sources Madgine signalslot connection.cpp connection.h connectionmanager.cpp connectionmanager.h connectionstore.cpp connectionstore.h slot.h signal.h signalrouter.h)
	FolderSources(sources Madgine util log.cpp log.h process.cpp process.h standardlog.cpp standardlog.h traceback.h utilmethods.cpp utilmethods.h)
FolderSources(sources "" vstools Madgine.natvis)

add_library(Interfaces SHARED ${sources})

add_precompiled_header(Interfaces ../../src/Madgine/interfaceslib.h FORCEINCLUDE SOURCE_CXX interfaceslib.cpp)

target_compile_definitions(Interfaces PRIVATE CONFIG_SUFFIX="$<$<CONFIG:Debug>:_d>")

target_link_libraries (Interfaces PRIVATE lualib ${CMAKE_THREAD_LIBS_INIT})


if (WIN32)
	target_link_libraries (Interfaces PRIVATE ws2_32)	
endif()

if (UNIX)
	target_link_libraries (Interfaces PRIVATE stdc++fs dl)
endif (UNIX)
 
if (BUILD_TESTING)
	add_subdirectory(test)	
endif()

install_to_workspace(Madgine TARGETS Interfaces EXPORT_LIB)

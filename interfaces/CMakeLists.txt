cmake_minimum_required (VERSION 2.6)
cmake_policy(SET CMP0011 OLD)
cmake_policy(SET CMP0049 OLD)
cmake_policy(SET CMP0048 NEW)

Project(Interfaces)

set(lua_include "" CACHE PATH "")
set(lua_lib "" CACHE FILEPATH "")

set(sources interfaceslib.cpp)
FolderSources(sources "" "")
FolderSources(sources "" src interfacesforward.h interfaceslib.h)
	FolderSources(sources src generic exceptionmessages.h keyvalue.cpp keyvalue.h message.h singleton.h templates.h valuetype.cpp valuetype.h)
	FolderSources(sources src math vector3.cpp vector3.h vector2.cpp vector2.h)
	FolderSources(sources src network linuxapi.cpp networkbuffer.cpp networkbuffer.h networkmanager.cpp networkmanager.h networkstream.cpp networkstream.h socketapi.h winapi.cpp)
	FolderSources(sources src scripting scriptingexception.h)
		FolderSources(sources scripting datatypes argumentlist.cpp argumentlist.h luatable.cpp luatable.h)
		FolderSources(sources scripting parsing parseexception.h scriptparser.cpp scriptparser.h)
		FolderSources(sources scripting types api.cpp api.h apihelper.cpp apihelper.h globalscopebase.cpp globalscopebase.h luastate.cpp luastate.h mapper.h scopebase.cpp scopebase.h scope_cast.h scope.h)
	FolderSources(sources src serialize fileexception.h observable.cpp observable.h serializable.cpp serializable.h serializableunit.cpp serializableunit.h serializeexception.h serializemanager.cpp serializemanager.h toplevelserializableunit.cpp toplevelserializableunit.h)
		FolderSources(sources serialize container action.h container.h creationhelper.h list.h map.h observed.h observablecontainer.h serializablecontainer.h serialized.h set.h sortedcontainer.h unithelper.cpp unithelper.h unsortedcontainer.h vector.h)
		FolderSources(sources serialize streams buffered_streambuf.cpp buffered_streambuf.h bufferedstream.cpp bufferedstream.h filebuffer.cpp filebuffer.h serializestream.cpp serializestream.h)
			FolderSources(sources streams debugging streamdebugging.cpp streamdebugging.h)
	FolderSources(sources src signalslot connection.cpp connection.h connectionstore.cpp connectionstore.h slot.h signal.h signalrouter.h)
	FolderSources(sources src util process.cpp process.h processlistener.h standardlog.cpp standardlog.h traceback.h utilmethods.cpp utilmethods.h)

include_directories(${Interfaces_SOURCE_DIR}/src)
add_library(Interfaces SHARED ${sources})
add_precompiled_header(Interfaces src/interfaceslib.h FORCEINCLUDE SOURCE_CXX interfaceslib.cpp)

target_include_directories(Interfaces PRIVATE ${lua_include})

target_link_libraries (Interfaces PRIVATE ${lua_lib} ${CMAKE_THREAD_LIBS_INIT})

if (WIN32)
	target_link_libraries (Interfaces PRIVATE ws2_32)	
endif()

if (CMAKE_COMPILER_IS_GNUCXX)
	target_link_libraries (Interfaces PRIVATE stdc++fs)
endif (CMAKE_COMPILER_IS_GNUCXX)
 
if (BUILD_TESTS)	
	add_subdirectory(test)	
endif(BUILD_TESTS)

installToWorkspace(Interfaces Madgine)

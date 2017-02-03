cmake_minimum_required (VERSION 2.6)
cmake_policy(SET CMP0011 OLD)
cmake_policy(SET CMP0049 OLD)


Project(INTERFACES)

set(CMAKE_MODULE_PATH
  "${CMAKE_MODULE_PATH}"
)


set(sources "")
FolderSources(sources "" "")
FolderSources(sources "" src interfacesforward.h interfaceslib.h singleton.h templates.cpp templates.h valuetype.cpp valuetype.h)
	FolderSources(sources src Network networkbuffer.cpp networkbuffer.h networkexception.cpp networkexception.h networkmanager.cpp networkmanager.h networkstream.cpp networkstream.h)
	FolderSources(sources src Scripting scriptingexception.h)
		FolderSources(sources Scripting Datatypes fieldaccessor.cpp fieldaccessor.h)
		FolderSources(sources Scripting Parsing entitynode.cpp entitynode.h methodnode.cpp methodnode.h parseexception.h prototype.cpp prototype.h scriptparser.cpp scriptparser.h textResource.cpp textResource.h)
		FolderSources(sources Scripting Statements accessor.cpp accessor.h ArithmeticalOperator.cpp ArithmeticalOperator.h assignment.cpp assignment.h comparator.cpp comparator.h constvalue.cpp constvalue.h for.cpp for.h if.cpp if.h logicaloperator.cpp logicaloperator.h methodcall.cpp methodcall.h negate.cpp negate.h return.cpp return.h scopesetter.cpp scopesetter.h statement.h while.cpp while.h)
		FolderSources(sources Scripting Types api.cpp api.h globalscope.h list.cpp list.h refscope.cpp refscope.h refscopetoplevelserializableunit.cpp refscopetoplevelserializableunit.h scope.cpp scope.h scope_cast.h scopeimpl.h struct.cpp struct.h vector.cpp vector.h)
	FolderSources(sources src Serialize fileexception.h observable.cpp observable.h serializable.cpp serializable.h serializablebase.h serializableunit.cpp serializableunit.h serializeexception.h serializemanager.cpp serializemanager.h toplevelserializableunit.cpp toplevelserializableunit.h)
		FolderSources(sources Serialize Container action.h basecontainer.h container.h creationhelper.h list.h map.h observed.h observablecontainer.h serializablecontainer.h serialized.h set.h sortedcontainer.h unithelper.h unsortedcontainer.h vector.h)
		FolderSources(sources Serialize Streams buffered_streambuf.cpp buffered_streambuf.h bufferedstream.cpp bufferedstream.h filebuffer.cpp filebuffer.h serializestream.cpp serializestream.h)
	FolderSources(sources src Util Process.cpp Process.h ProcessListener.h Traceback.h UtilMethods.cpp UtilMethods.h)

include_directories(${INTERFACES_SOURCE_DIR}/src)

add_library(Interfaces SHARED ${sources} interfaceslib.cpp)

add_precompiled_header(Interfaces src/interfaceslib.h FORCEINCLUDE SOURCE_CXX interfaceslib.cpp)

target_link_libraries (Interfaces ws2_32)

 
if (BUILD_TESTS)
	add_subdirectory(test)
endif(BUILD_TESTS)
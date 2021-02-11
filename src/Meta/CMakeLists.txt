cmake_minimum_required (VERSION 2.6)

Project(Meta)


set(sources)

FolderSources(sources "" "" metaforward.h metalib.h)
	FolderSources(sources "" keyvalue accessor.h apifunction.cpp apifunction.h boundapifunction.cpp boundapifunction.h functiontable.cpp functiontable.h keyvalueiterator.h keyvaluevirtualiterator.h metatable.cpp metatable.h nativeobject.cpp nativeobject.h objectfieldaccessor.cpp objectfieldaccessor.h objectptr.cpp objectptr.h ownedscopeptr.cpp ownedscopeptr.h scopefield.cpp scopefield.h scopeiterator.cpp scopeiterator.h typedscopeptr.cpp typedscopeptr.h valuetype.cpp valuetype.h valuetype_desc.cpp valuetype_desc.h valuetype_forward.cpp valuetype_forward.h valuetype_types.h valuetypeexception.h)
	FolderSources(sources "" keyvalueutil valuetypeserialize.cpp valuetypeserialize.h)
	FolderSources(sources "" math atlas2.cpp atlas2.h boundingbox.h bounds.cpp bounds.h geometry3.cpp geometry3.h matrix4.cpp matrix4.h matrix3.cpp matrix3.h plane.h quaternion.cpp quaternion.h ray.h transformation.cpp transformation.h vector4.h vector3.h vector2.h)
	FolderSources(sources "" serialize creationhelper.h fileexception.h formatter.cpp formatter.h syncable.cpp syncable.h serializable.h serializableids.h serializableunit.cpp serializableunit.h serializableunitptr.cpp serializableunitptr.h serializeexception.h serializemanager.cpp serializemanager.h syncmanager.cpp syncmanager.h syncableunit.cpp syncableunit.h toplevelunit.cpp toplevelunit.h serializetable.cpp serializetable.h unithelper.cpp unithelper.h)
		FolderSources(sources serialize container action.h synced.h syncablecontainer.h serializablecontainer.h serialized.h)
		FolderSources(sources serialize formatter safebinaryformatter.cpp safebinaryformatter.h)
		FolderSources(sources serialize streams buffered_streambuf.cpp buffered_streambuf.h bufferedstream.cpp bufferedstream.h serializestream.cpp serializestream.h serializestreambuf.cpp serializestreambuf.h wrappingserializestreambuf.cpp wrappingserializestreambuf.h serializablemapholder.cpp serializablemapholder.h)
	

add_workspace_library(Meta ${sources} SOURCE_ROOT .. PRECOMPILED_HEADER metalib.h)

target_link_libraries(Meta PUBLIC Generic)

install_to_workspace(Madgine TARGETS Meta EXPORT_LIB)

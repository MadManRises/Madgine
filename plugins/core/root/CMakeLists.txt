Project(Root)

set(sources "")
FolderSources(sources "" src)
	FolderSources(sources src Madgine rootforward.h rootlib.h)
		FolderSources(sources Madgine root keyvalueregistry.cpp keyvalueregistry.h root.cpp root.h rootcomponentbase.cpp rootcomponentbase.h rootcomponentcollector.cpp rootcomponentcollector.h)

add_plugin(Root Madgine Core ${sources} PRECOMPILED_HEADER src/Madgine/rootlib.h)

target_link_libraries(Root PUBLIC CLI CodeGen Modules)


Project(Behavior)

set(sources "")
FolderSources(sources "" src)
	FolderSources(sources src Madgine behaviorforward.h behaviorlib.h behavior.cpp behavior.h behaviorcollector.cpp behaviorcollector.h behaviortracker.cpp behaviortracker.h nativebehaviorcollector.cpp nativebehaviorcollector.h)
		
add_plugin(Behavior Madgine Core ${sources} PRECOMPILED_HEADER src/Madgine/behaviorlib.h)

target_link_libraries(Behavior PUBLIC Meta Modules)


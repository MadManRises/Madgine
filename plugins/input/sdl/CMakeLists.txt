

Project(SDLInput)

set(sources "")
FolderSources(sources "" src sdlinputhandler.cpp sdlinputhandler.h sdlinputlib.h)


add_plugin(SDLInput Madgine Input ${sources} PRECOMPILED_HEADER src/sdlinputlib.h)


target_link_plugins(SDLInput Client)
target_link_libraries(SDLInput PRIVATE SDL)

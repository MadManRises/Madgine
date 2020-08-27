

Project(AndroidInput)

set(sources "")
FolderSources(sources "" src directinputhandler.cpp directinputhandler.h directinputlib.h)


add_plugin(DirectInput Madgine Input ${sources} PRECOMPILED_HEADER src/directinputlib.h)

target_link_plugins(DirectInput Client)

target_link_libraries(DirectInput PRIVATE )

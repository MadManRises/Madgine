

Project(EmscriptenInput)

set(sources "")
FolderSources(sources "" src emscripteninputhandler.cpp emscripteninputhandler.h emscripteninputlib.h)


add_plugin(EmscriptenInput Madgine Input ${sources} PRECOMPILED_HEADER src/emscripteninputlib.h)



target_link_plugins(EmscriptenInput Client)

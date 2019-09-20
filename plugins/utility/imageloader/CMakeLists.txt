
Project(ImageLoader)

set(sources "")
FolderSources(sources "" "" imagedata.h imageloader.cpp imageloader.h imageloaderlib.h)


add_plugin(ImageLoader Madgine Utility ${sources} PRECOMPILED_HEADER imageloaderlib.h SOURCE_ROOT .)

target_link_libraries(ImageLoader PUBLIC Modules stb)

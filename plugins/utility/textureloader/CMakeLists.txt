
Project(TextureLoader)

set(sources "")
FolderSources(sources "" "" texture.h textureloader.cpp textureloader.h textureloaderlib.h)


add_plugin(TextureLoader Madgine Utility ${sources} PRECOMPILED_HEADER textureloaderlib.h SOURCE_ROOT .)

target_link_libraries(TextureLoader PUBLIC Modules)
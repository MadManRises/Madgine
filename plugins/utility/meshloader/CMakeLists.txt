
Project(MeshLoader)

set(sources "")
FolderSources(sources "" "" meshdata.h meshloader.cpp meshloader.h meshloaderlib.h)


add_plugin(MeshLoader Madgine Utility ${sources} PRECOMPILED_HEADER meshloaderlib.h SOURCE_ROOT .)

target_link_libraries(MeshLoader PUBLIC Modules)
target_link_libraries(MeshLoader PRIVATE assimp)
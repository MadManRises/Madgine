
Project(MeshLoader)

set(sources "")
FolderSources(sources "" "" gpumeshdata.h gpumeshloader.cpp gpumeshloader.h meshdata.cpp meshdata.h meshloader.cpp meshloader.h meshloaderlib.h)


add_plugin(MeshLoader Madgine Utility ${sources} PRECOMPILED_HEADER meshloaderlib.h SOURCE_ROOT .)

target_link_libraries(MeshLoader PUBLIC Render)
target_link_libraries(MeshLoader PRIVATE assimp)

target_link_plugins(MeshLoader Resources)
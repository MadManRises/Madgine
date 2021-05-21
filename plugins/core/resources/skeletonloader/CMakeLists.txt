
Project(SkeletonLoader)

set(sources "")
FolderSources(sources "" "" skeletondescriptor.h skeletonloader.cpp skeletonloader.h skeletonloaderlib.h)


add_plugin(SkeletonLoader Madgine Utility ${sources} PRECOMPILED_HEADER skeletonloaderlib.h SOURCE_ROOT .)

target_link_libraries(SkeletonLoader PRIVATE assimp)

target_link_plugins(SkeletonLoader Resources)

Project(AnimationLoader)

set(sources "")
FolderSources(sources "" "" animationdescriptor.h animationloader.cpp animationloader.h animationloaderlib.h)


add_plugin(AnimationLoader Madgine Utility ${sources} PRECOMPILED_HEADER animationloaderlib.h SOURCE_ROOT .)

target_link_libraries(AnimationLoader PRIVATE assimp)

target_link_plugins(AnimationLoader SkeletonLoader Resources)
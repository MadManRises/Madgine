
Project(TextureLoader)

set(sources "")
FolderSources(sources "" Madgine textureloaderlib.h)
	FolderSources(sources Madgine textureloader textureloader.cpp textureloader.h)


add_plugin(TextureLoader Madgine Utility ${sources} PRECOMPILED_HEADER Madgine/textureloaderlib.h SOURCE_ROOT .)

target_link_libraries(TextureLoader PUBLIC Render)

target_link_plugins(TextureLoader ImageLoader)
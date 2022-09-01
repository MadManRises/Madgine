
Project(FontLoader)

set(sources "")
FolderSources(sources "" Madgine fontloaderlib.h)
	FolderSources(sources Madgine fontloader font.h fontloader.cpp fontloader.h)


add_plugin(FontLoader Madgine Utility ${sources} PRECOMPILED_HEADER Madgine/fontloaderlib.h SOURCE_ROOT .)

target_link_plugins(FontLoader TextureLoader)

target_link_libraries(FontLoader PUBLIC FilesystemSerialize)
target_link_libraries(FontLoader PRIVATE freetype lib_msdfgen)
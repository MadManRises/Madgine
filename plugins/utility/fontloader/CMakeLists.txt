
Project(FontLoader)

set(sources "")
FolderSources(sources "" "" font.h fontloader.cpp fontloader.h fontloaderlib.h)


add_plugin(FontLoader Madgine Utility ${sources} PRECOMPILED_HEADER fontloaderlib.h SOURCE_ROOT .)

target_link_plugins(FontLoader TextureLoader)

target_link_libraries(FontLoader PUBLIC Modules stb Filesystem)
target_link_libraries(FontLoader PRIVATE freetype lib_msdfgen)

Project(FontLoader)

set(sources "")
FolderSources(sources "" "" font.h fontloader.cpp fontloader.h fontloaderlib.h)


add_plugin(FontLoader Madgine Utility ${sources} PRECOMPILED_HEADER fontloaderlib.h SOURCE_ROOT .)

target_link_libraries(FontLoader PUBLIC Modules stb)

install_to_workspace(Utility EXPORT_LIB TARGETS FontLoader)
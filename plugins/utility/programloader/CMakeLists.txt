
Project(ProgramLoader)

set(sources "")
FolderSources(sources "" "" program.h programloader.cpp programloader.h programloaderlib.h)


add_plugin(ProgramLoader Madgine Utility ${sources} PRECOMPILED_HEADER programloaderlib.h SOURCE_ROOT .)

target_link_libraries(ProgramLoader PUBLIC Modules)

install_to_workspace(Utility EXPORT_LIB TARGETS ProgramLoader)
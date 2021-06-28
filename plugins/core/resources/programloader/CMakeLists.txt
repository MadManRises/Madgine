
Project(ProgramLoader)

set(sources "")
FolderSources(sources "" "" program.h programloader.cpp programloader.h programloaderlib.h)


add_plugin(ProgramLoader Madgine Utility ${sources} PRECOMPILED_HEADER programloaderlib.h SOURCE_ROOT .)

target_link_plugins(ProgramLoader Resources)
target_link_libraries(ProgramLoader PUBLIC CodeGen)
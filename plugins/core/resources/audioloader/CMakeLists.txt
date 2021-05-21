
Project(AudioLoader)

set(sources "")
FolderSources(sources "" "" audiobuffer.h audiostream.h audiobufferloader.cpp audiobufferloader.h audiostreamloader.cpp audiostreamloader.h audioloaderlib.h oggdecode.cpp oggdecode.h oggdecodebuf.cpp oggdecodebuf.h)


add_plugin(AudioLoader Madgine Utility ${sources} PRECOMPILED_HEADER audioloaderlib.h SOURCE_ROOT .)

target_link_libraries(AudioLoader PUBLIC vorbisfile)

target_link_plugins(AudioLoader Resources)
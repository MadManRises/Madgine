
Project(PipelineLoader)

set(sources "")
FolderSources(sources "" "" pipeline.h pipelineinstance.h pipelineloader.cpp pipelineloader.h pipelineloaderlib.h)


add_plugin(PipelineLoader Madgine Utility ${sources} PRECOMPILED_HEADER pipelineloaderlib.h SOURCE_ROOT .)

target_link_plugins(PipelineLoader Resources)
target_link_libraries(PipelineLoader PUBLIC CodeGen)

Project(PipelineLoader)

set(sources "")
FolderSources(sources "" Madgine pipelineloaderlib.h)
	FolderSources(sources Madgine pipelineloader pipeline.h pipelineinstance.h pipelineloader.cpp pipelineloader.h)


add_plugin(PipelineLoader Madgine Utility ${sources} PRECOMPILED_HEADER Madgine/pipelineloaderlib.h SOURCE_ROOT .)

target_link_plugins(PipelineLoader Resources MeshLoader)
target_link_libraries(PipelineLoader PUBLIC CodeGen)
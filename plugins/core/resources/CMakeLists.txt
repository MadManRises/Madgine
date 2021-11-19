
PROJECT(Resources)

set(sources "")
FolderSources(sources "" src)
	FolderSources(sources src Madgine)
		FolderSources(sources Madgine resources resourcebase.cpp resourcebase.h resourceloaderbase.cpp resourceloaderbase.h resourceloadercollector.cpp resourceloadercollector.h resourceloader.cpp resourceloader.h resourcemanager.cpp resourcemanager.h)

add_plugin(Resources Madgine Core ${sources})

target_link_libraries(Resources PUBLIC Modules Meta)


if (NOT IOS)
	add_subdirectory(audioloader)
endif()
add_subdirectory(imageloader)
add_subdirectory(programloader)
add_subdirectory(textureloader)
add_subdirectory(meshloader)
add_subdirectory(fontloader)
add_subdirectory(skeletonloader)
add_subdirectory(animationloader)

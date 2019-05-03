cmake_minimum_required (VERSION 2.6)

Project(OpenGL)

set(sources "")
FolderSources(sources "" src)
FolderSources(sources src glad glad.c glad.h)
FolderSources(sources src OpenGL openglforward.h opengllib.h openglmesh.cpp openglmesh.h openglmeshdata.h openglmeshloader.cpp openglmeshloader.h openglrenderer.cpp openglrenderer.h openglrendertexture.cpp openglrendertexture.h openglrenderwindow.cpp openglrenderwindow.h openglshaderloader.cpp openglshaderloader.h openglshaderprogram.cpp openglshaderprogram.h)
	FolderSources(sources OpenGL util openglbuffer.cpp openglbuffer.h openglshader.cpp openglshader.h opengltexture.cpp opengltexture.h)

add_plugin(OpenGL Madgine Renderer ${sources} PRECOMPILED_HEADER src/OpenGL/opengllib.h)

target_link_plugins(OpenGL Client)
target_link_libraries(OpenGL PRIVATE assimp)

if (NOT ANDROID)
	find_package(OpenGL REQUIRED)
else()
	set(OPENGL_LIBRARIES EGL GLESv3)
	set(OPENGL_INCLUDE_DIR "")
endif()
	
target_include_directories(OpenGL PUBLIC ${OPENGL_INCLUDE_DIR})

target_link_libraries(OpenGL PUBLIC ${OPENGL_LIBRARIES})


if (BUILD_TESTING)
	add_subdirectory(test)
endif()

cmake_minimum_required (VERSION 2.6)

Project(OpenGL)

set(sources "")
FolderSources(sources "" src)
if (NOT ANDROID AND NOT EMSCRIPTEN)
	FolderSources(sources src glad glad.c glad.h)
endif()
FolderSources(sources src OpenGL openglfontdata.h openglfontloader.cpp openglfontloader.h openglforward.h opengllib.h openglmesh.cpp openglmesh.h openglmeshdata.h openglmeshloader.cpp openglmeshloader.h openglrenderer.cpp openglrenderer.h openglrendertexture.cpp openglrendertexture.h openglrenderwindow.cpp openglrenderwindow.h openglshaderloader.cpp openglshaderloader.h openglshaderprogram.cpp openglshaderprogram.h)
	FolderSources(sources OpenGL util openglbuffer.cpp openglbuffer.h openglshader.cpp openglshader.h opengltexture.cpp opengltexture.h openglvertexarray.cpp openglvertexarray.h)

add_plugin(OpenGL Madgine Renderer ${sources} PRECOMPILED_HEADER src/OpenGL/opengllib.h)

target_link_plugins(OpenGL Client)
target_link_libraries(OpenGL PUBLIC assimp freetype lib_msdfgen) # Might be a bug in MSVC + Clang. Should be PRIVATE

if (NOT ANDROID)
	find_package(OpenGL REQUIRED)
else()
	set(OPENGL_LIBRARIES EGL GLESv3)
	set(OPENGL_INCLUDE_DIR "")
endif()

if (EMSCRIPTEN)
	set(OPENGL_LIBRARIES ${OPENGL_gl_LIBRARIES})
endif()
	
target_include_directories(OpenGL PUBLIC ${OPENGL_INCLUDE_DIR})

target_link_libraries(OpenGL PUBLIC ${OPENGL_LIBRARIES})


if (BUILD_TESTING)
	add_subdirectory(test)
endif()

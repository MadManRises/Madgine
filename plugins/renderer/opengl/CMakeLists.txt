cmake_minimum_required (VERSION 2.6)

cmake_policy(SET CMP0049 OLD)
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0022 NEW)

Project(OpenGL)



set(sources "")
FolderSources(sources "" src)
FolderSources(sources src OpenGL glad.c glad.h openglcontextguard.cpp openglcontextguard.h openglforward.h opengllib.h openglrenderer.cpp openglrenderer.h openglrendertexture.cpp openglrendertexture.h openglrenderwindow.cpp openglrenderwindow.h openglshader.cpp openglshader.h openglshaderloader.cpp openglshaderloader.h openglshaderprogram.cpp openglshaderprogram.h opengltexture.cpp opengltexture.h)


add_plugin(OpenGL Madgine Renderer ${sources} PRECOMPILED_HEADER src/OpenGL/opengllib.h)

target_link_plugins(OpenGL PUBLIC Client)

if (NOT ANDROID)
	find_package(OpenGL REQUIRED)
else()
	set(OPENGL_LIBRARIES EGL)
	set(OPENGL_INCLUDE_DIR "")
endif()
	
target_include_directories(OpenGL PRIVATE ${OPENGL_INCLUDE_DIR})

target_link_libraries(OpenGL PRIVATE ${OPENGL_LIBRARIES})


if (BUILD_TESTING)
	add_subdirectory(test)
endif()

cmake_minimum_required (VERSION 2.6)

Project(OpenGL)



if (NOT ANDROID)
	find_package(OpenGL REQUIRED)
else()
	set(OPENGL_LIBRARIES EGL GLESv3)
	set(OPENGL_INCLUDE_DIR "")
endif()

if (EMSCRIPTEN)
	set(OPENGL_LIBRARIES ${OPENGL_gl_LIBRARIES})
endif()
	


set(sources "")
FolderSources(sources "" src)
if (NOT ANDROID AND NOT EMSCRIPTEN)
	FolderSources(sources src glad glad.c glad.h)
endif()
FolderSources(sources src OpenGL openglfontdata.h openglfontloader.cpp openglfontloader.h openglforward.h opengllib.h openglmeshdata.h openglmeshloader.cpp openglmeshloader.h openglprogramloader.cpp openglprogramloader.h openglrendertexture.cpp openglrendertexture.h openglrenderwindow.cpp openglrenderwindow.h openglshaderloader.cpp openglshaderloader.h opengltextureloader.cpp opengltextureloader.h openglrendertarget.cpp openglrendertarget.h openglrendercontext.cpp openglrendercontext.h)
	FolderSources(sources OpenGL util gldump.cpp openglbuffer.cpp openglbuffer.h openglshader.cpp openglshader.h opengltexture.cpp opengltexture.h openglvertexarray.cpp openglvertexarray.h openglprogram.cpp openglprogram.h)

add_plugin(OpenGL Madgine Renderer ${sources} PRECOMPILED_HEADER src/OpenGL/opengllib.h)

target_link_plugins(OpenGL Client)
target_link_libraries(OpenGL PRIVATE freetype lib_msdfgen)
target_link_libraries(OpenGL PRIVATE ${OPENGL_LIBRARIES})
target_include_directories(OpenGL PRIVATE ${OPENGL_INCLUDE_DIR})


set(sources "")
FolderSources(sources "" tools)
	FolderSources(sources tools OpenGL_Tools opengltoolconfig.cpp opengltoolconfig.h opengltoolsforward.h opengltoolslib.h)
		FolderSources(sources OpenGL_Tools imgui imgui_impl_opengl3.cpp imgui_impl_opengl3.h openglimroot.cpp openglimroot.h)


add_tools(OpenGL ${sources} PRECOMPILED_HEADER tools/OpenGL_Tools/opengltoolslib.h)

target_link_plugins(OpenGLTools ClientTools)
target_link_libraries(OpenGLTools PRIVATE ${OPENGL_LIBRARIES})
target_include_directories(OpenGLTools PRIVATE ${OPENGL_INCLUDE_DIR})

if (BUILD_TESTING)
	add_subdirectory(test)
endif()

cmake_minimum_required (VERSION 2.6)

Project(OpenGL)



if (NOT ANDROID AND NOT IOS AND NOT OSX)
	find_package(OpenGL REQUIRED)
elseif(ANDROID)
	set(OPENGL_LIBRARIES EGL GLESv3)
	set(OPENGL_INCLUDE_DIR "")
elseif(OSX)
    set(OPENGL_LIBRARIES "-framework Cocoa")
    set(OPENGL_INCLUDE_DIR "")
elseif(IOS)
    set(OPENGL_LIBRARIES "-framework UIKit" "-framework GLKit" "-framework OpenGLES")
    set(OPENGL_INCLUDE_DIR "")
endif()

if (EMSCRIPTEN)
	set(OPENGL_LIBRARIES ${OPENGL_gl_LIBRARIES})
endif()
	


set(sources "")
FolderSources(sources "" src)
if (NOT ANDROID AND NOT EMSCRIPTEN AND NOT IOS)
	FolderSources(sources src glad glad.c glad.h)
endif()
FolderSources(sources src OpenGL openglforward.h opengllib.h openglmeshdata.h openglmeshloader.cpp openglmeshloader.h openglprogramloader.cpp openglprogramloader.h openglrendertexture.cpp openglrendertexture.h openglrenderwindow.cpp openglrenderwindow.h openglshaderloader.cpp openglshaderloader.h opengltextureloader.cpp opengltextureloader.h openglrendertarget.cpp openglrendertarget.h openglrendercontext.cpp openglrendercontext.h)
	FolderSources(sources OpenGL util gldump.cpp openglbuffer.cpp openglbuffer.h openglshader.cpp openglshader.h openglssbobuffer.cpp openglssbobuffer.h opengltexture.cpp opengltexture.h openglvertexarray.cpp openglvertexarray.h openglprogram.cpp openglprogram.h)

add_plugin(OpenGL Madgine Renderer ${sources} PRECOMPILED_HEADER src/OpenGL/opengllib.h)

target_link_libraries(OpenGL PRIVATE ${OPENGL_LIBRARIES} Client)
target_include_directories(OpenGL PRIVATE ${OPENGL_INCLUDE_DIR})

if (OSX)
    target_sources(OpenGL PRIVATE src/OpenGL/osxopengl.mm)
elseif(IOS)
    target_sources(OpenGL PRIVATE src/OpenGL/iosopengl.mm)
endif ()

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

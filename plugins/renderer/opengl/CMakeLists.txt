cmake_minimum_required (VERSION 2.6)

cmake_policy(SET CMP0049 OLD)
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0022 NEW)

Project(OpenGL)



set(sources "")
FolderSources(sources "" src)
FolderSources(sources src OpenGL glad.cpp glad.h openglforward.h opengllib.h openglsystem.cpp openglsystem.h opengltoplevelwindow.cpp opengltoplevelwindow.h)


add_plugin(OpenGL Madgine Renderer ${sources})

add_precompiled_header(OpenGL src/OpenGL/opengllib.h FORCEINCLUDE)

find_package(OpenGL REQUIRED)

target_include_directories(OpenGL PRIVATE ${OPENGL_INCLUDE_DIR})

target_link_libraries(OpenGL PUBLIC Client PRIVATE ${OPENGL_LIBRARIES})

if (BUILD_TESTING)
	add_subdirectory(test)
endif()

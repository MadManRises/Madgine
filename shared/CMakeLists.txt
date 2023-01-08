SET(CMAKE_DEBUG_POSTFIX "" CACHE STRING "" FORCE) #Some libs set this value

set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_FBX_IMPORTER ON)
set(ASSIMP_BUILD_COLLADA_IMPORTER ON)
set(ASSIMP_BUILD_STL_IMPORTER ON)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(assimp-5.2.5)

install_to_workspace(assimp TARGETS assimp EXPORT_LIB)

export_to_workspace(assimp)

set(DISABLE_FORCE_DEBUG_POSTFIX ON)
set(FT_DISABLE_ZLIB ON CACHE BOOL "" FORCE)
#comment definition of FT_CONFIG_OPTION_USE_ZLIB out in ftconfig.h
add_subdirectory(freetype-2.12.1)
set_target_properties(freetype PROPERTIES EXCLUDE_FROM_ALL TRUE)

add_subdirectory(msdfgen)
set_target_properties(lib_msdfgen PROPERTIES EXCLUDE_FROM_ALL TRUE)

add_subdirectory(imgui)
set_target_properties(ImGui PROPERTIES EXCLUDE_FROM_ALL TRUE)
target_include_directories(ImGui PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

add_subdirectory(NodeEditor)
set_target_properties(imgui_node_editor imgui_canvas PROPERTIES EXCLUDE_FROM_ALL TRUE)

add_subdirectory(im3d)
set_target_properties(Im3D PROPERTIES EXCLUDE_FROM_ALL TRUE)

add_subdirectory(Madgine)

add_subdirectory(steamworks)

add_subdirectory(EOS)

######################## Static Libs #################

set(OLD_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})
set(OLD_BUILD_TESTING ${BUILD_TESTING})

set(USE_MSVC_RUNTIME_LIBRARY_DLL ${BUILD_SHARED_LIBS} CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF)
set(BUILD_TESTING OFF)

set (BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set (BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
set (BUILD_BULLET2_DEMOS OFF CACHE BOOL "" FORCE)
add_subdirectory(bullet3-3.24)
target_include_directories(LinearMath PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/bullet3-3.24/src>)

install_to_workspace(BulletLib TARGETS BulletDynamics BulletCollision LinearMath EXPORT_LIB)

export_to_workspace(BulletLib)

if (NOT IOS AND NOT EMSCRIPTEN)
	add_subdirectory(libogg-1.3.4)

	add_library(Ogg::ogg ALIAS ogg)

	add_subdirectory(libvorbis-1.3.7)
endif()

set(BUILD_SHARED_LIBS ${OLD_BUILD_SHARED_LIBS})
set(BUILD_TESTING ${OLD_BUILD_TESTING})



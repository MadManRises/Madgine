set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_FBX_IMPORTER ON)
set(ASSIMP_BUILD_COLLADA_IMPORTER ON)
add_subdirectory(assimp)

set(DISABLE_FORCE_DEBUG_POSTFIX ON)
add_subdirectory(freetype-2.10.1)

add_subdirectory(msdfgen)

add_subdirectory(imgui)

add_subdirectory(NodeEditor)

add_subdirectory(im3d)

add_subdirectory(stb)


add_subdirectory(SDL2-2.0.9)
if (BUILD_SHARED_LIBS)
	add_library(SDL ALIAS SDL2)
else()
	add_library(SDL ALIAS SDL2-static)
endif()


export_to_workspace(Shared)

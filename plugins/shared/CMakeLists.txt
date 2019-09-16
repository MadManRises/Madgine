set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_FBX_IMPORTER ON)
add_subdirectory(assimp)

set(DISABLE_FORCE_DEBUG_POSTFIX ON)
add_subdirectory(freetype-2.10.1)

add_subdirectory(msdfgen)
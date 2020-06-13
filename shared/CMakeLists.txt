set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_FBX_IMPORTER ON)
set(ASSIMP_BUILD_COLLADA_IMPORTER ON)
add_subdirectory(assimp)

set(DISABLE_FORCE_DEBUG_POSTFIX ON)
add_subdirectory(freetype-2.10.1)
set_target_properties(freetype PROPERTIES EXCLUDE_FROM_ALL TRUE)

add_subdirectory(msdfgen)
set_target_properties(lib_msdfgen PROPERTIES EXCLUDE_FROM_ALL TRUE)

add_subdirectory(imgui)
set_target_properties(ImGui PROPERTIES EXCLUDE_FROM_ALL TRUE)

add_subdirectory(NodeEditor)
set_target_properties(imgui_node_editor imgui_canvas PROPERTIES EXCLUDE_FROM_ALL TRUE)

add_subdirectory(im3d)
set_target_properties(Im3D PROPERTIES EXCLUDE_FROM_ALL TRUE)

add_subdirectory(stb)
set_target_properties(stb PROPERTIES EXCLUDE_FROM_ALL TRUE)


add_subdirectory(SDL2-2.0.9)
if (TARGET SDL2-static)
	set_target_properties(SDL2-static PROPERTIES EXCLUDE_FROM_ALL TRUE)
endif()
if (TARGET SDL2)
	set_target_properties(SDL2 PROPERTIES EXCLUDE_FROM_ALL TRUE)
endif()
if (TARGET SDL2main)
	set_target_properties(SDL2main PROPERTIES EXCLUDE_FROM_ALL TRUE)
endif()
if (BUILD_SHARED_LIBS)
	add_library(SDL ALIAS SDL2)
else()
	add_library(SDL ALIAS SDL2-static)
endif()

if (NOT ANDROID AND NOT EMSCRIPTEN)
	option(OIS_BUILD_SHARED_LIBS "Build shared libraries" ${BUILD_SHARED_LIBS})
	option(OIS_BUILD_DEMOS "Build demo applications" OFF)

	add_subdirectory(ois)
	set_target_properties(OIS PROPERTIES EXCLUDE_FROM_ALL TRUE)
	if (BUILD_SHARED_LIBS)
		set_target_properties(OIS PROPERTIES POSITION_INDEPENDENT_CODE ON)
	endif()
endif()

export_to_workspace(Shared)

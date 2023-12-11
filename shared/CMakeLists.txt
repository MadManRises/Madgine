include(Legal)


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

add_notices(assimp 
	READABLE_NAME "Asset Importer" 
	LICENSE_FILE assimp-5.2.5/LICENSE 
	CREDITS "Model-Loading: Open Asset Import Library (assimp) - https://github.com/assimp/assimp")


set(DISABLE_FORCE_DEBUG_POSTFIX ON)
set(FT_DISABLE_ZLIB ON CACHE BOOL "" FORCE)
#comment definition of FT_CONFIG_OPTION_USE_ZLIB out in ftconfig.h
add_subdirectory(freetype-2.12.1)
set_target_properties(freetype PROPERTIES EXCLUDE_FROM_ALL TRUE)

add_notices(freetype 
	READABLE_NAME FreeType
	LICENSE_FILE freetype-2.12.1/docs/FTL.txt 
	CREDITS "Font-Loading: FreeType - https://freetype.org/")


add_subdirectory(msdfgen)
set_target_properties(lib_msdfgen PROPERTIES EXCLUDE_FROM_ALL TRUE)

add_notices(lib_msdfgen 
	READABLE_NAME msdfgen 
	LICENSE_FILE msdfgen/LICENSE.txt 
	CREDITS "Font-Generation: Multi-channel signed distance field generator - https://github.com/Chlumsky/msdfgen")


add_subdirectory(imgui)
set_target_properties(ImGui PROPERTIES EXCLUDE_FROM_ALL TRUE)
target_include_directories(ImGui PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_link_options(ImGui PUBLIC "/NATVIS:${CMAKE_CURRENT_LIST_DIR}/imgui/misc/debuggers/imgui.natvis")

add_notices(ImGui 
	LICENSE_FILE imgui/LICENSE.txt 
	CREDITS "Tooling-UI: Dear ImGui - https://github.com/ocornut/imgui")


add_subdirectory(NodeEditor)
set_target_properties(imgui_node_editor imgui_canvas PROPERTIES EXCLUDE_FROM_ALL TRUE)

add_notices(imgui_node_editor 
	READABLE_NAME "ImGui Node Editor" 
	LICENSE_FILE NodeEditor/LICENSE 
	CREDITS "Nodegraph-Editor: Node Editor in ImGui - https://github.com/thedmd/imgui-node-editor")


add_subdirectory(im3d)
set_target_properties(Im3D PROPERTIES EXCLUDE_FROM_ALL TRUE)


add_subdirectory(Madgine)


add_subdirectory(steamworks)


add_subdirectory(EOS)

set(BUILD_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(zep)
set_target_properties(Zep PROPERTIES EXCLUDE_FROM_ALL TRUE)
target_include_directories(Zep PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/zep/include>)

add_notices(Zep 
	READABLE_NAME "Zep" 
	LICENSE_FILE zep/LICENSE 
	CREDITS "TextEditor: Zep - https://github.com/Rezonality/zep")


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

add_notices(LinearMath 
	READABLE_NAME Bullet3D 
	CREDITS "Physics-Simulation: Bullet3D - http://bulletphysics.org")


if (NOT IOS AND NOT EMSCRIPTEN)
	add_subdirectory(libogg-1.3.4)

	add_library(Ogg::ogg ALIAS ogg)

	add_notices(ogg 
		READABLE_NAME Ogg 
		LICENSE_FILE libogg-1.3.4/COPYING 
		CREDITS "Multimedia-Format: Ogg - https://xiph.org/ogg/")


	add_subdirectory(libvorbis-1.3.7)

	add_notices(vorbis 
		READABLE_NAME Vorbis 
		LICENSE_FILE libvorbis-1.3.7/COPYING 
		CREDITS "Audio-Compression: Vorbis - https://xiph.org/vorbis/")
endif()

set(PA_BUILD_STATIC ON CACHE BOOL "" FORCE)
set(PA_BUILD_SHARED OFF CACHE BOOL "" FORCE)
add_subdirectory(portaudio-19.7.0)

add_notices(portaudio_static 
		READABLE_NAME PortAudio
		LICENSE_FILE portaudio-19.7.0/LICENSE.txt 
		CREDITS "Audio-IO: PortAudio - https://www.portaudio.com/")

set(BUILD_SHARED_LIBS ${OLD_BUILD_SHARED_LIBS})
set(BUILD_TESTING ${OLD_BUILD_TESTING})



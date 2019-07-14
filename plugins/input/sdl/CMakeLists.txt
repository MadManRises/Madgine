
add_subdirectory(SDL2-2.0.9)
if (STATIC_BUILD)
	set(SDL_LIB SDL2-static)
else()
	set(SDL_LIB SDL2)
endif()



Project(SDLInput)

set(sources "")
FolderSources(sources "" src sdlinputhandler.cpp sdlinputhandler.h sdlinputlib.h)


add_plugin(SDLInput Madgine Input ${sources} PRECOMPILED_HEADER src/sdlinputlib.h)


target_link_libraries(SDLInput PUBLIC Client)
target_link_libraries(SDLInput PRIVATE ${SDL_LIB})

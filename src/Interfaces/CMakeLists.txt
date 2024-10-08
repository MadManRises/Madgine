Project(Interfaces VERSION 1.0.0)

option(INTERFACES_ENABLE_PROFILER "Activates the builtin runtime-profiler." ON)
option(INTERFACES_ENABLE_MEMTRACKING "Enables Memory Tracking for Heap-Allocations")


set(sources)

FolderSources(sources "" "" interfacesforward.h interfaceslib.h)
	FolderSources(sources "" debug stacktrace.cpp stacktrace.h traceback.cpp traceback.h)
		FolderSources(sources debug memory memory.cpp memory.h statsmemoryresource.cpp statsmemoryresource.h untrackedmemoryresource.cpp untrackedmemoryresource.h)
	FolderSources(sources "" dl dlapi_unix.cpp dlapi_win.cpp dlapi.h runtime.cpp runtime.h runtime_unix.cpp runtime_win.cpp)
	FolderSources(sources "" filesystem api.cpp api.h api_android.cpp api_emscripten.cpp api_linux.cpp api_osx.cpp api_win.cpp filequery.cpp filequery_android.cpp filequery_emscripten.cpp filequery_linux.cpp filequery_osx.cpp filequery_ios.cpp filequery_win.cpp filequery.h filewatcher_win.cpp filewatcher_emscripten.cpp filewatcher_linux.cpp filewatcher_android.cpp filewatcher_osx.cpp filewatcher_ios.cpp filewatcher.cpp filewatcher.h path.cpp path.h)
	FolderSources(sources "" input rawinput_win.cpp rawinput_win.h)
	FolderSources(sources "" socket socketapi.h socketapi_unix.cpp socketapi_win.cpp)
	FolderSources(sources "" threading threadapi.h threadapi_posix.cpp threadapi_win.cpp)
	FolderSources(sources "" util log.cpp log.h standardlog.cpp standardlog.h utilmethods.cpp utilmethods.h)
	FolderSources(sources "" window windowapi.h windowapi_android.cpp windowapi_emscripten.cpp windowapi_linux.cpp windowapi_win.cpp windoweventlistener.h)

add_workspace_library(Interfaces ${sources} SOURCE_ROOT .. PRECOMPILED_HEADER interfaceslib.h)
    
configure_file(interfacesconfig.in.h interfacesconfig.h)
target_include_directories(Interfaces PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)

target_link_libraries(Interfaces PUBLIC Generic)

if (WINDOWS)
	target_link_libraries (Interfaces PRIVATE ws2_32 DbgHelp)	
	if (MINGW)
		if (NOT BUILD_SHARED_LIBS)
			target_link_libraries (Interfaces PRIVATE -Wl,-Bstatic,--whole-archive pthread -Wl,-Bdynamic,--no-whole-archive)
		else()
			target_link_libraries (Interfaces PRIVATE pthread)
		endif()
	endif ()
endif()

if (ANDROID)
	target_link_libraries(Interfaces PRIVATE android EGL log)	
endif()

if (UNIX)
	if (EMSCRIPTEN)
		add_custom_command(TARGET Interfaces POST_BUILD
			COMMAND npm install ws bufferutil utf-8-validate
			WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
		target_link_libraries(Interfaces PRIVATE -lidbfs.js)
	elseif (ANDROID)
	elseif (OSX)
        target_sources(Interfaces PRIVATE window/windowapi_osx.mm)
        set_source_files_properties(window/windowapi_osx.mm PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
        target_link_libraries(Interfaces PRIVATE "-framework Cocoa")
    elseif (IOS)
        target_sources(Interfaces PRIVATE window/windowapi_ios.mm filesystem/api_ios.mm)
		set_source_files_properties(window/windowapi_ios.mm filesystem/api_ios.mm PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
        target_link_libraries(Interfaces PRIVATE "-framework UIKit -framework Foundation")
	elseif (LINUX)
		find_package(X11 REQUIRED)
		find_package(OpenGL REQUIRED)
		target_link_libraries (Interfaces PRIVATE dl X11 OpenGL::GLX)
	else ()
		MESSAGE(SEND_ERROR "Unknown UNIX-Platform!")
	endif ()
endif (UNIX)

install_to_workspace(Madgine TARGETS Interfaces EXPORT_LIB)

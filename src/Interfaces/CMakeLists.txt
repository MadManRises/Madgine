cmake_minimum_required (VERSION 2.6)

Project(Interfaces)

CMAKE_DEPENDENT_OPTION(INTERFACES_ENABLE_THREADING "Enable multithreading utilities like thread local or workgroups" ON
                       "NOT EMSCRIPTEN" OFF)

CMAKE_DEPENDENT_OPTION(INTERFACES_USE_CUSTOM_THREADLOCAL "Replace the thread_local keyword with a custom implementation for thread-local variables." ON "INTERFACES_ENABLE_THREADING" OFF)
option(INTERFACES_USE_PTHREAD_THREADLOCAL_STORE "Use pthread_[set/get]specific instead of thread_local to store all thread-local variables. Useful in combination with INTERFACES_USE_CUSTOM_THREADLOCAL on platforms that don't support thread_local.")

option(INTERFACES_ENABLE_PROFILER "Activates the builtin runtime-profiler." ON)
option(INTERFACES_ENABLE_MEMTRACKING "Enables Memory Tracking for Heap-Allocations")


set(sources)

FolderSources(sources "" "" commonlib.h exception.cpp exception.h interfacesforward.h interfaceslib.h stringutil.h)
	FolderSources(sources "" debug stacktrace.cpp stacktrace.h traceback.cpp traceback.h)
		FolderSources(sources debug memory memory.cpp memory.h statsmemoryresource.cpp statsmemoryresource.h untrackedmemoryresource.cpp untrackedmemoryresource.h)
	FolderSources(sources "" dl dlapi_unix.cpp dlapi_win.cpp dlapi.h runtime.cpp runtime.h runtime_unix.cpp runtime_win.cpp)
	FolderSources(sources "" filesystem api.cpp api.h api_android.cpp api_emscripten.cpp api_linux.cpp api_win.cpp filequery.cpp filequery_android.cpp filequery_emscripten.cpp filequery_linux.cpp filequery_win.cpp filequery.h path.cpp path.h)
	FolderSources(sources "" socket socketapi.h socketapi_unix.cpp socketapi_win.cpp)
	FolderSources(sources "" streams streams.cpp streams.h)
	FolderSources(sources "" threading threadapi.h threadapi_unix.cpp threadapi_win.cpp)
	FolderSources(sources "" util log.cpp log.h standardlog.cpp standardlog.h utilmethods.cpp utilmethods.h)
	FolderSources(sources "" window windowapi.h windowapi_android.cpp windowapi_emscripten.cpp windowapi_linux.cpp windowapi_win.cpp windoweventlistener.h)

add_workspace_library(Interfaces ${sources} SOURCE_ROOT .. PRECOMPILED_HEADER interfaceslib.h)

configure_file(interfacesconfig.in.h interfacesconfig.h)
target_include_directories(Interfaces PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)


if (WIN32)
	target_link_libraries (Interfaces PRIVATE ws2_32 DbgHelp)	
endif()

if (ANDROID)
	target_link_libraries(Interfaces PRIVATE android EGL log)	
endif()

if (UNIX)
	if (EMSCRIPTEN)
		add_custom_command(TARGET Interfaces POST_BUILD
			COMMAND npm install ws bufferutil utf-8-validate
			WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
	elseif (ANDROID)
	else ()
		find_package(X11 REQUIRED)
		find_package(OpenGL REQUIRED)
		target_link_libraries (Interfaces PRIVATE dl X11 OpenGL::GLX)
	endif()
endif (UNIX)

install_to_workspace(Madgine TARGETS Interfaces EXPORT_LIB)

cmake_minimum_required (VERSION 2.6)

cmake_policy(SET CMP0011 OLD)
cmake_policy(SET CMP0049 OLD)
cmake_policy(SET CMP0022 NEW)

set(BOOST_PATH "" CACHE PATH "Path to Boost (tested with 1_63)")
if (BOOST_PATH STREQUAL "")
    message (FATAL_ERROR "Assertion failed: Boost is required!")
endif () 


find_package(Threads REQUIRED)

set (sources "")
FolderSources(sources "" "" appcontrol.cpp appcontrol.h applicationinfo.h errorcodes.cpp errorcodes.h inputinfo.h maditorsharedlib.cpp maditorsharedlib.h maditorsharedforward.h moduleinstance.cpp moduleinstance.h projectfile.cpp projectfile.h sharedbase.h sharedmemory.cpp sharedmemory.h)
	FolderSources(sources "" ipcmanager boostipcbuffer.cpp boostipcbuffer.h boostipcconnection.cpp boostipcconnection.h boostipcmanager.cpp boostipcmanager.h boostipcserver.cpp boostipcserver.h boostipcstream.cpp boostipcstream.h)

include_directories(${Maditor_Shared_SOURCE_DIR})

add_library(Maditor_Shared SHARED ${sources})
add_precompiled_header(Maditor_Shared maditorsharedlib.h FORCEINCLUDE SOURCE_CXX maditorsharedlib.cpp)

target_include_directories(Maditor_Shared PUBLIC ${BOOST_PATH})

target_link_libraries(Maditor_Shared PUBLIC Interfaces)

target_link_libraries(Maditor_Shared PRIVATE ${CMAKE_THREAD_LIBS_INIT})

if (CMAKE_COMPILER_IS_GNUCXX)
	target_link_libraries(Maditor_Shared PRIVATE rt)
endif (CMAKE_COMPILER_IS_GNUCXX)

install_to_workspace(Maditor TARGETS Maditor_Shared)




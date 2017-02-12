cmake_minimum_required (VERSION 2.6)

cmake_policy(SET CMP0011 OLD)
cmake_policy(SET CMP0049 OLD)
cmake_policy(SET CMP0048 NEW)

Project(SERVER)

set(CMAKE_MODULE_PATH
  "${CMAKE_MODULE_PATH}"
)


set(sources "")
FolderSources(sources "" "" serverlib.cpp)
FolderSources(sources "" src serverforward.h serverlib.h)
	FolderSources(sources src Server baseserver.cpp baseserver.h serverlog.cpp serverlog.h)

include_directories(${SERVER_SOURCE_DIR}/src ${BASE_SOURCE_DIR}/src ${INTERFACES_INCLUDE_DIRS})

add_library(MadgineServer SHARED ${sources} serverlib.cpp)

target_link_libraries(MadgineServer Base Interfaces)

add_precompiled_header(MadgineServer src/serverlib.h FORCEINCLUDE SOURCE_CXX serverlib.cpp)

 
if (BUILD_TESTS)
	add_subdirectory(test)
endif(BUILD_TESTS)
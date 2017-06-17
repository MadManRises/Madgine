set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

project(Madgine_Test)

include_directories(${gtest_SOURCE_DIR}/include)
include_directories(${gmock_SOURCE_DIR}/include)

set(sources "")
FolderSources(sources "" "" integrationtests.cpp)
FolderSources(sources "" mocks)

add_executable(Madgine_Test ${sources})

target_link_libraries(Madgine_Test OgreMadgine ${GTEST_LIBS})
project(Madgine_Test)

include_directories(${gtest_SOURCE_DIR}/include)

set(sources "")
FolderSources(sources "" "" main.cpp)

add_executable(Madgine_Test ${sources})

target_link_libraries(Madgine_Test Madgine gtest_main)
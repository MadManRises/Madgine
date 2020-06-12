
project(Interfaces_Test)

add_library(Interfaces_gtest_main STATIC Interfaces_gtest_main.cc)

target_link_libraries(Interfaces_gtest_main PRIVATE Interfaces gtest)

set(sources "")
FolderSources(sources "" "")
	FolderSources(sources "" filesystem pathtest.cpp)

add_gtest(Interfaces_Test ${sources})

target_link_libraries(Interfaces_Test PRIVATE Interfaces)

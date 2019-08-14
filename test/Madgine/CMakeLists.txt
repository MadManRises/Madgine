
project(Madgine_Test)

set(sources "")
FolderSources(sources "" "" integrationtests.cpp)
FolderSources(sources "" mocks)

add_gtest(Madgine_Test ${sources})

target_link_all_plugins(Madgine_Test)
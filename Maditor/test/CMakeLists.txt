
project(Maditor_Test)

include_directories(${Madgine_SOURCE_DIR}/Maditor)

set(sources "")
FolderSources(sources "" "" integrationtests.cpp)
FolderSources(sources "" mocks dialogmanagermock.h)

add_gtestmock(Maditor_Test ${sources})

target_link_libraries(Maditor_Test PRIVATE Maditor_Model)

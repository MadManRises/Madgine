
project(Network_Test)

set(sources networkmanagertest.cpp)

add_gtest(Network_Test ${sources})

target_link_libraries(Network_Test PRIVATE Network)

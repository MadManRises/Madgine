
project(Network_Test)

set(sources networkmanagertest.cpp ../../../../test/Meta/Meta_gtest_main.cc)

add_gtest(Network_Test ${sources})

target_link_libraries(Network_Test PRIVATE Network)
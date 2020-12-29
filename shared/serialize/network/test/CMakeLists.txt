
project(Network_Test)

set(sources networkmanagertest.cpp)

add_gtest(Network_Test ${sources})

target_link_libraries(Network_Test PRIVATE Network)
target_link_plugins(Network_Test) #has to be there to a generated .cpp to static build
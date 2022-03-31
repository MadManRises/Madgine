

Project(MMOLobby)

set(sources "")

FolderSources(sources "" src server.cpp server.h)	

add_plugin(MMOLobby MMOLobby Game ${sources} PRECOMPILED_HEADER src/mmolobbylib.h)

target_link_plugins(MMOLobby Server)
target_link_libraries(MMOLobby PRIVATE NetworkSerialize)

add_executable(MMOLobbyServer src/server_main.cpp)

generate_binary_info(MMOLobbyServer)

target_link_plugins(MMOLobbyServer MMOLobby Base Client)
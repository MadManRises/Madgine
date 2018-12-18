
add_executable(MadgineLauncher launcher.cpp)

target_link_plugins(MadgineLauncher PRIVATE Client Tools OISHandler OpenGL)

install_to_workspace(Madgine TARGETS MadgineLauncher)
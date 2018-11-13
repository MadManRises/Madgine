
add_executable(MadgineLauncher launcher.cpp ${STATIC_BUILD})

target_link_libraries(MadgineLauncher PRIVATE Client Tools)

install_to_workspace(Madgine TARGETS MadgineLauncher)
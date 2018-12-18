
if (STATIC_BUILD)
	get_static_config_file(components_source components ".cpp")
endif()

add_executable(MadgineLauncher launcher.cpp ${components_source})

target_link_plugins(MadgineLauncher PRIVATE Client Tools OISHandler OpenGL)

install_to_workspace(Madgine TARGETS MadgineLauncher)

include (android)

if (NOT CMAKE_ANDROID_ARCH_ABI)
	add_executable(MadgineLauncher launcher.cpp)
else()
	add_library(MadgineLauncher SHARED androidLauncher.cpp launcher.cpp)
	build_apk(MadgineLauncher)
endif()

target_link_plugins(MadgineLauncher PRIVATE Client Tools OISHandler OpenGL)

install_to_workspace(Madgine TARGETS MadgineLauncher)
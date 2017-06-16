cmake_minimum_required (VERSION 2.6)
cmake_policy(SET CMP0011 OLD)
cmake_policy(SET CMP0049 OLD)
cmake_policy(SET CMP0048 NEW)



Project(UNITYMADGINEWRAPPER)

add_library(UnityMadgineWrapper SHARED SceneManager.cs)

include_external_msproject(
    UnityMadgineWrapper UnityMadgineWrapper.csproj
    TYPE FAE04EC0-301F-11D3-BF4B-00C04F79EFBC)

FILE(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" DOS_STYLE_SOURCE_DIR)

CONFIGURE_FILE(UnityMadgineWrapper.csproj.in ${CMAKE_BINARY_DIR}/UnityMadgineWrapper.csproj)

if (BUILD_TESTS)
	add_subdirectory(test)
endif(BUILD_TESTS)
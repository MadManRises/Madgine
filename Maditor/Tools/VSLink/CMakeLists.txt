cmake_minimum_required (VERSION 2.6)

Project(VSLink)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

add_definitions(-DCMAKE_VS_DEVENV_COMMAND="${CMAKE_VS_DEVENV_COMMAND}")

include_directories(${Maditor_SOURCE_DIR}/src)
include_directories(${MADGINE_SOURCE_DIR}/src)

add_library(VSLink SHARED VSCommands.cs VSLink.cpp VSLink.h VSMsg.h VSSettingsWidget.cpp VSSettingsWidget.h VSSettingsWidget.ui)

target_link_libraries(VSLink Qt5::Widgets Maditor)

set(TOOLS_LIBS ${TOOLS_LIBS} VSLink PARENT_SCOPE)
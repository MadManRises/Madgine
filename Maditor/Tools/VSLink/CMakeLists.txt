cmake_minimum_required (VERSION 2.6)

Project(VSLink)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set (CMAKE_AUTOUIC ON)

set (CMAKE_AUTOMOC_MOC_OPTIONS "-bmaditorviewlib.h")

add_definitions(-DCMAKE_VS_DEVENV_COMMAND="${CMAKE_VS_DEVENV_COMMAND}")

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${BOOST_PATH})
include_directories(${Maditor_Model_SOURCE_DIR})
include_directories(${Maditor_View_SOURCE_DIR})
include_directories(${MADGINE_INCLUDE_DIRS})

add_library(VSLink SHARED VSCommands.cs VSLink.cpp VSLink.h VSLinkView.cpp VSLinkView.h VSMsg.h VSSettingsWidget.cpp VSSettingsWidget.h VSSettingsWidget.ui)

target_link_libraries(VSLink Qt5::Widgets Maditor_Model Maditor_View)

set(TOOLS_BINS ${TOOLS_BINS} $<TARGET_FILE:VSLink> PARENT_SCOPE)
set(TOOLS_LIBS ${TOOLS_LIBS} VSLink PARENT_SCOPE)
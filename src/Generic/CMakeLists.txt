cmake_minimum_required (VERSION 2.6)

Project(Generic)

add_workspace_interface_library(Generic SOURCE_ROOT ..)

install_interface_to_workspace(Madgine TARGETS Generic SOURCE_ROOT ..)

Project(Generic VERSION 1.0.0)

add_workspace_interface_library(Generic SOURCE_ROOT ..)

target_link_options(Generic INTERFACE "/NATVIS:${CMAKE_CURRENT_LIST_DIR}/Generic.natvis")

install_interface_to_workspace(Madgine TARGETS Generic SOURCE_ROOT ..)

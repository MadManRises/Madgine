Project(Server)

set(sources serverbase.cpp serverbase.h serverinstance.cpp serverinstance.h serverlog.cpp serverlog.h)

add_workspace_library(Server ${sources} SOURCE_ROOT ../..)

target_link_libraries(Server PUBLIC Modules Meta)

install_to_workspace(Madgine EXPORT_LIB TARGETS Server)


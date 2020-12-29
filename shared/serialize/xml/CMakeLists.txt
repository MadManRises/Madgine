Project(Xml)

set(sources xmlformatter.cpp xmlformatter.h)
	
add_workspace_library(Xml ${sources} SOURCE_ROOT ../..)

target_link_libraries(Xml PUBLIC Modules)

install_to_workspace(Madgine EXPORT_LIB TARGETS Xml)
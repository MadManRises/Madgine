Project(CLI)

set(sources base64.cpp base64.h)
	
add_workspace_library(Base64 ${sources} SOURCE_ROOT ..)

target_link_libraries(Base64 PUBLIC Generic)

install_to_workspace(Madgine EXPORT_LIB TARGETS Base64)
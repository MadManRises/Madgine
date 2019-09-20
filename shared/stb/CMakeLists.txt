Project(STB)


set(sources "")
FolderSources(sources "" "" stb.cpp stb_image.h)

add_workspace_library(stb ${sources} SOURCE_ROOT .)

target_link_libraries(stb PUBLIC Modules)

install_to_workspace(Shared EXPORT_LIB TARGETS stb)
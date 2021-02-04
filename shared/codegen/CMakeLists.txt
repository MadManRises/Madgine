Project(CodeGen)

set(sources codegen.cpp codegen.h codegen_cpp.cpp codegen_cpp.h codegen_internal.h)
	
add_workspace_library(CodeGen ${sources} SOURCE_ROOT ..)

target_include_directories(CodeGen PRIVATE $<TARGET_PROPERTY:Modules,INTERFACE_INCLUDE_DIRECTORIES>)

install_to_workspace(Madgine EXPORT_LIB TARGETS CodeGen)